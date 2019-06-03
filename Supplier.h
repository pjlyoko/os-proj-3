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
	Supplier(int numb, mutex *mutexFridge, int *fridge, mutex *mutexWriter);

	~Supplier();

	void threadSupplier();

	void threadClose();

private:
	bool end = false;
	float breaks = 500000;
	thread *threadS;
	int numb;
	mutex *mutexFridge, *mutexWriter;
	int *fridge;
};

#endif /* SUPPLIER_H */

