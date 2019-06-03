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
#include <algorithm>

using namespace std;

#ifndef COOK_H
#define COOK_H

class Pizzaiolo {
public:
	Pizzaiolo(int numb, mutex *mutexOrdersList, vector<Order *> *ordersList, mutex *mutexFridge, int *fridge,
			  mutex *mutexTools, bool *tools, mutex *mutexFurnaces, int *furnace, mutex *mutexCountertop,
			  Pizza **countertop, mutex *mutexWriter);

	Pizzaiolo(const Pizzaiolo &orig);

	void threadStart();

	void threadClose();

	~Pizzaiolo();

private:
	bool end = false;
	int breaks = 500000;
	int numb;
	vector<int> ingredients;
	thread *threadC;

	mutex *mutexOrdersList, *mutexFridge, *mutexTools, *mutexFurnaces, *mutexCountertop, *mutexWriter;
	int *fridge;
	bool *tools;
	int *furnaces;
	Pizza **countertop;
	vector<Order *> *ordersList;
};

#endif /* COOK_H */

