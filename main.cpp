#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <semaphore>
using namespace std;

mutex mtx;
const unsigned MAX_CLIENTS = 15;    // Number of clients
const unsigned PACK_OF_TICKETS = 5;     // Pack of tickets
std::counting_semaphore emptyStock(0);  // Empty stock's semaphore
std::counting_semaphore fullStock(0);   // Full stock's semaphore
unsigned ticketsAvailable = 0;  // Tickets availables

/**
 * Decrement the number of tickets available in the booth
 * @brief getTicketFromStock
 */
void getTicketFromStock() {
    --ticketsAvailable;
}

/**
 * Refill the number of tickets in the booth
 * @brief putTicketsInStock
 */
void putTicketsInStock() {
    ticketsAvailable = PACK_OF_TICKETS;
}

/**
 * Actions of a seller
 * @brief seller
 */
void seller(){
    while (true){
        emptyStock.acquire();   // lock the emptyStock's semaphore (lock the clients who wants to buy a ticket)

        putTicketsInStock();
        cout << "Put " << PACK_OF_TICKETS << " in stocks" << endl;

        fullStock.release();    // unlock the fullStock's semaphore (unlock the clients who are waiting)

        usleep(100000); // The seller refill the booth during 0.1 seconds
    }
}

/**
 * Actions of a client
 * @brief client
 * @param i
 */
void client(unsigned i){
    while (true){
        mtx.lock(); // Lock the other clients
        if (ticketsAvailable == 0){
            cout << "Waiting for ticket " << i << endl;
            emptyStock.release();   // wait during the refillement
            fullStock.acquire();
        }
        getTicketFromStock();
        mtx.unlock();   // Unlock the other clients

        cout << "Ticket bought by " << i << endl;
        usleep(100000); // The client buy the ticket during 0.1 seconds
    }
}

/**
 * Main program
 * @brief main
 * @return
 */
int main()
{
    unsigned clients = MAX_CLIENTS; // There is MAX_CLIENTS clients
    vector<thread> clients_threads; // Booth's queue

    thread sellerThread(seller);    // Seller

    for (unsigned j = 0; j < clients; ++j) {    // Execute all clients thread
        clients_threads.emplace_back(thread(client, j));
    }

    sellerThread.join();    // Synchronize the seller thread

    for (thread &thread : clients_threads) {    // Synchronize all the clients thread
        thread.join();
    }
    return 0;
}
