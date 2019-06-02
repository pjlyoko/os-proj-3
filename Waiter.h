#include "Pizza.h"
#include "Client.h"
#include <thread>
#include <mutex>
#include <ncurses.h>
#include <ctime>
#include <unistd.h>
#include <cmath>
#include <chrono>
#include <vector>

using namespace std;

#ifndef WAITER_H
#define WAITER_H

class Waiter {
public:
    Waiter();

    Waiter(const Waiter &orig);

    Waiter(int numb, mutex *mutexCountertop, Pizza **countertop, vector<Client *> clients);

    virtual ~Waiter();

    void threadWaiter();

    void threadClose();

private:
    bool end;
    Pizza *pizza;
    float breaks;
    thread *threadW;

    int numb;
    mutex *mutexCountertop;
    Pizza **countertop;
    vector<Client *> clients;

};

#endif /* WAITER_H */

