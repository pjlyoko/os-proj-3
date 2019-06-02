#include "Order.h"
#include "Pizza.h"
#include <thread>
#include <mutex>
#include <ncurses.h>
#include <ctime>
#include <unistd.h>
#include <cmath>
#include <chrono>

using namespace std;

#ifndef SUPPLIER_H
#define SUPPLIER_H

class Supplier {
public:
    Supplier();

    Supplier(const Supplier &orig);

    Supplier(int numb, mutex *mutexFridge, int *fridge);

    virtual ~Supplier();

    void threadSuplier();

    void threadClose();

private:
    bool end;
    float breaks;
    thread *threadS;
    int numb;
    mutex *mutexFridge;
    int *fridge;
};

#endif /* SUPPLIER_H */

