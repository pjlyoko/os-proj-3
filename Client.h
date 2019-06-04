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
#include <condition_variable>

using namespace std;

#ifndef CLIENT_H
#define CLIENT_H

class Chair;

class Client {
public:
	Client(int numb, mutex *mutexChairs, vector<Chair *> *chairs,
			mutex *mutexOrdersList, vector<Order *> *ordersList, condition_variable *cvClientMadeAnOrder,
			mutex *mutexWriter,
			vector<Client *> *otherClients);

	~Client();

	void threadClient();

	void threadClose();

	void print_queue(queue<Order *> q);

	void setPizza(Pizza *p);

	inline int getClientType() { return clientType; };

	inline int getClientNumber() { return numb; };

private:
	Pizza *pizza = nullptr;
	bool end = false;
	int breaks = 500000;
	int clientType = random() % 2;

	thread *threadC;
	int numb;
	mutex *mutexChairs, *mutexOrdersList, *mutexWriter;
	vector<Order *> *ordersList;
	vector<Client *> *otherClients;
	vector<Chair *> *chairs;
	condition_variable *cvClientMadeAnOrder;

	int chairTaken = -1;
	bool oughtToLeave = false;

	void printChairs();

	void takeASeat();
	void makeOrder();
	void waitForDelivery();
	void eat();
	void leave();
	void leaveImmediately();
};

#endif /* CLIENT_H */

