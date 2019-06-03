#include "Pizzaiolo.h"
#include "Client.h"
#include "Waiter.h"
#include "Supplier.h"
#include "Pizza.h"
#include <vector>
#include <iostream>
#include <thread>
#include <ncurses.h>

using namespace std;

int main() {
	srand(time(nullptr));
	initscr();
	noecho();
	curs_set(0);
	char x;

	vector<Pizzaiolo *> pizzaiolos;
	vector<Client *> clients;
	vector<Waiter *> waiters;

	auto ordersList = new vector<Order *>();
	mutex mutexOrdersList, mutexFridge, mutexTools, mutexFurnaces, mutexCountertop, mutexChairs, mutexWriter;
	int fridge[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	bool tools[4]{
			true, true, true, true
	};
	int furnace[5]{
			2, 2, 2, 2, 2
	};
	Pizza *countertop[10]{
			nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
	};

	bool chairs[10]{
			true, true, true, true, true, true, true, true, true, true
	};
	mvprintw(0, 0, "Dziala");

	//tworzenie wątków
	clients.reserve(11);
	for(int i = 0; i < 11; i++) {
		clients.push_back(new Client(i, &mutexChairs, chairs, &mutexOrdersList, ordersList, &mutexWriter));
	}

	pizzaiolos.reserve(7);
	for(int i = 0; i < 7; i++) {
		pizzaiolos.push_back(
				new Pizzaiolo(i, &mutexOrdersList, ordersList, &mutexFridge, fridge, &mutexTools, tools, &mutexFurnaces,
							  furnace, &mutexCountertop, countertop, &mutexWriter));
	}

	waiters.reserve(2);
	for(int i = 0; i < 2; i++) {
		waiters.push_back(new Waiter(i, &mutexCountertop, countertop, clients, &mutexWriter));
	}

	do {
		timeout(3);
		x = getch();

		if(x == 'c') {
			clear();
			x = ' ';
		}
	} while(x != 'x');

	//Zamykanie wątków i konczenie
	for(int i = pizzaiolos.size() - 1; i > 0; i--) {
		pizzaiolos[i]->threadClose();
		pizzaiolos.pop_back();
	}


	for(int i = clients.size() - 1; i > 0; i--) {
		clients[i]->threadClose();
		clients.pop_back();
	}

	for(int i = waiters.size() - 1; i > 0; i--) {
		waiters[i]->threadClose();
		waiters.pop_back();

	}

	endwin();
	return 0;
}

