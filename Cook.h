#include "Order.h"
#include "Pizza.h"
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <ncurses.h>
#include <ctime>
#include <unistd.h>
#include <cmath>
#include <chrono>

using namespace std;

#ifndef COOK_H
#define COOK_H

class Cook {
public:
    Cook();

    Cook(int numb, mutex *mutexOrdersList, vector<Order *> *ordersList, mutex *mutexFridge, int *fridge,
         mutex *mutexTools, bool *tools, mutex *mutexFurnaces, int *furnace, mutex *mutexCountertop,
         Pizza **countertop);

    Cook(const Cook &orig);

    void threadCook();

    void threadClose();


    virtual ~Cook();

private:
    int numb;
    bool end;
    vector<int> ingredients;
    int breaks;
    thread *threadC;

    mutex *mutexOrdersList, *mutexFridge, *mutexTools, *mutexFurnaces, *mutexCountertop;
    int *fridge;
    bool *tools;
    int *furnaces;
    Pizza **countertop;
    vector<Order *> *ordersList;
};

#endif /* COOK_H */

