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
	Waiter(int numb, mutex *mutexCountertop, Pizza **countertop, vector<Client *> clients, mutex *mutexWriter);

	~Waiter();

	void threadWaiter();

	void threadClose();

private:
	bool end = false;
	int breaks = 500000;
	Pizza *pizza = nullptr;

	thread *threadW;
	int numb;
	mutex *mutexCountertop, *mutexWriter;
	Pizza **countertop;
	vector<Client *> clients;

	void takePizza(int counterSize);
	void deliverPizza();
};

#endif /* WAITER_H */

