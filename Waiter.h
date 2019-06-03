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
#include <condition_variable>

using namespace std;

#ifndef WAITER_H
#define WAITER_H

class Waiter {
public:
	Waiter(int numb, mutex *mutexCountertop, Pizza **countertop, vector<Client *> clients, mutex *mutexWriter, condition_variable *cv);

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
	condition_variable *cv;

	bool pizzasAvailable(int counterSize);

	void takePizza(int counterSize);
	void deliverPizza();
};

#endif /* WAITER_H */

