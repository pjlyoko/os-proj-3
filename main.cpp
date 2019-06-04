#include "Pizzaiolo.h"
#include "Client.h"
#include "Waiter.h"
#include "Pizza.h"
#include "Chair.h"
#include <vector>
#include <iostream>
#include <thread>
#include <ncurses.h>
#include <csignal>
#include <condition_variable>

using namespace std;

void windowResizeHandler(int) {
	endwin();
	refresh();
	clear();
}

int main() {
	signal(SIGWINCH, windowResizeHandler);

	srandom(time(nullptr));
	auto win = initscr();
	curs_set(0);
	if(has_colors()) {
		start_color();
		init_pair(1, COLOR_BLACK, COLOR_WHITE);
		init_pair(2, COLOR_WHITE, COLOR_RED);
		init_pair(3, COLOR_BLACK, COLOR_GREEN);
		init_pair(4, COLOR_WHITE, COLOR_BLUE);
	}

	char x;

	int x_max = 0, y_max = 0;

	getmaxyx(win, y_max, x_max);
	if(x_max < 100 || y_max < 30) {
		while(x_max < 100 || y_max < 30) {
			mvprintw(y_max / 2, x_max / 2 - 12, "Powieksz okno terminala.");
			getmaxyx(win, y_max, x_max);
			sleep(1);
		}
		windowResizeHandler(0);
	}


	vector<Pizzaiolo *> pizzaiolos;
	vector<Client *> clients;
	vector<Waiter *> waiters;
	vector<Order *> ordersList;
	vector<Chair *> chairs;
	condition_variable cv, cvClientMadeAnOrder;


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



	// Tworzenie krzeseł
	chairs.reserve(10);
	for(int i = 0; i < 10; i++) {
		chairs.push_back(new Chair());
	}

	// Tworzenie klientów
	clients.reserve(11);
	for(int i = 0; i < 11; i++) {
		clients.push_back(
				new Client(i, &mutexChairs, &chairs, &mutexOrdersList, &ordersList, &cvClientMadeAnOrder, &mutexWriter,
						   &clients));
	}

	// Tworzenie pizzaiolo
	pizzaiolos.reserve(5);
	for(int i = 0; i < 5; i++) {
		pizzaiolos.push_back(
				new Pizzaiolo(i, &mutexOrdersList, &ordersList, &cvClientMadeAnOrder, &mutexFridge, fridge, &mutexTools,
							  tools, &mutexFurnaces, furnace, &mutexCountertop, countertop, &mutexWriter, &cv));
	}

	// Tworzenie kelnerów
	waiters.reserve(2);
	for(int i = 0; i < 2; i++) {
		waiters.push_back(new Waiter(i, &mutexCountertop, countertop, clients, &mutexWriter, &cv));
	}

	do {
		timeout(3);
		x = getch();

		if(x == 'c') {
			clear();
			x = ' ';
		}
	} while(x != 'x' && x != 'q');

	// Zamykanie wątków i konczenie
	for(int i = waiters.size() - 1; i > 0; i--) {
		waiters[i]->threadClose();
		waiters.pop_back();
	}

	for(int i = pizzaiolos.size() - 1; i > 0; i--) {
		pizzaiolos[i]->threadClose();
		pizzaiolos.pop_back();
	}

	for(int i = clients.size() - 1; i > 0; i--) {
		clients[i]->threadClose();
		clients.pop_back();
	}

	for(int i = chairs.size() - 1; i > 0; i--) {
		chairs.pop_back();
	}

	endwin();
	return 0;
}

