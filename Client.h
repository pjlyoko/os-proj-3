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
	Client(int numb, mutex *mutexChairs, bool *chairs, mutex *mutexOrdersList, vector<Order *> *ordersList,
		   mutex *mutexWriter);

	~Client();

	void threadClient();

	void threadClose();

	void print_queue(queue<Order *> q);

	void setPizza(Pizza *p);

private:
	Pizza *pizza = nullptr;
	bool end = false;
	int breaks = 500000;

	thread *threadC;
	int numb;
	mutex *mutexChairs, *mutexOrdersList, *mutexWriter;
	bool *chairs;
	vector<Order *> *ordersList;

	int chairTaken = -1;

	void takeASeat(int chairsSize);
	void makeOrder();
	void waitForDelivery();
	void eat();
	void leave(int chairsSize);
};

#endif /* CLIENT_H */

