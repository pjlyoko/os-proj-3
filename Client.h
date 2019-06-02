#include "Order.h"
#include "Pizza.h"
#include <thread>
#include <mutex>
#include <ncurses.h>
#include <ctime>
#include <unistd.h>
#include <cmath>
#include <chrono>
#include <queue>
#include <vector>
#include <cstdlib>

using namespace std;

#ifndef CLIENT_H
#define CLIENT_H

class Client {
public:
    Client();

    Client(const Client &orig);

    Client(int numb, mutex *mutexChairs, bool *chairs, mutex *mutexOrdersList, vector<Order *> *ordersList);

    virtual ~Client();

    void threadClient();

    void threadClose();

    void print_queue(queue<Order *> q);

    void setPizza(Pizza *p);

private:
    Pizza *pizza;
    bool end;
    int breaks;
    thread *threadC;
    int numb;
    mutex *mutexChairs, *mutexOrdersList;
    bool *chairs;
    vector<Order *> *ordersList;
};

#endif /* CLIENT_H */

