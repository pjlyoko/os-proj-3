#include <utility>

#include "Client.h"

Client::Client(int numb, mutex *mutexChairs, bool *chairs, mutex *mutexOrdersList, vector<Order *> *ordersList,
			   mutex *mutexWriter) :
		numb(numb), mutexOrdersList(mutexOrdersList), ordersList(ordersList), mutexChairs(mutexChairs), chairs(chairs),
		mutexWriter(mutexWriter) {
	threadC = new thread(&Client::threadClient, this);
}

Client::~Client() {
	threadC->join();
	delete threadC;
}

void Client::print_queue(queue<Order *> q) {
	int i = 0;
	auto *que = new queue<Order *>(std::move(q));
	while(!que->empty()) {
		mvprintw(1, 50 + 3 * i, "%d", que->front()->getClient());
		i++;
		que->pop();
	}
	delete que;

}

void Client::takeASeat(int chairsSize) {
	bool hasChair = false;

	while(!hasChair && !end) {
		{
			unique_lock<mutex> lk_write(*mutexWriter);
			mvprintw(10 + numb, 0, "Klient %d: Szuka miejsca do siedzenia", numb);
		}

		usleep(breaks);


		{
			unique_lock<mutex> lk(*mutexChairs);

			for(int i = 0; i < chairsSize; i++) {
				if(chairs[i]) {
					hasChair = true;
					chairs[i] = false;
					chairTaken = i;

					{
						unique_lock<mutex> lk_write(*mutexWriter);

						mvprintw(25, 50, "Krzesla");
						for(int j = 0; j < chairsSize; j++) {
							mvprintw(26, 50 + 2 * j, "%d", (int) chairs[j]);
						}
					}

					break;
				}
			}
		}

		if(hasChair) {
			break;
		}
	}
}

void Client::threadClient() {
	bool haveChair;
	vector<int> ingredients;
	chrono::_V2::steady_clock::time_point begin, dur;
	float eatTime;
	Order *order;

	int chairsSize = 10;

	while(!end) {
		haveChair = false;
		eatTime = 1 + random() % 3;

		usleep(breaks);

		//Siadanie
		takeASeat(chairsSize);

		//Składanie zamówienia

		mutexWriter->lock();
		mvprintw(10 + numb, 0, "Klient %d: Sklada zamowienie           ", numb);
		mutexWriter->unlock();

		usleep(breaks);
		while(!mutexOrdersList->try_lock() && !end);

		ingredients.push_back(0); //ciasto i inne składniki
		int numberOfIngredients = random() % 4 + 3;
		for(int i = 1; i < numberOfIngredients; i++) {
			ingredients.push_back(random() % 9 + 1);
		}

		order = new Order(numb, random() % 2 + 1, ingredients);
		ordersList->push_back(order);

		ingredients.clear();
		mutexWriter->lock();
		for(int i = 0; i < 20; i++) {
			mvprintw(1, 50 + 3 * i, "   ");
		}
		mvprintw(0, 50, "Zamowienia");
		for(int i = 0; i < ordersList->size(); i++) {
			mvprintw(1, 50 + 3 * i, "%d", ordersList->operator[](i)->getClient());
		}
		mutexWriter->unlock();

		mutexOrdersList->unlock();
		usleep(breaks);

		//Czekanie
		mutexWriter->lock();
		mvprintw(10 + numb, 0, "Klient %d: Czeka na pizze           ", numb);
		mutexWriter->unlock();
		while(pizza == nullptr && !end) {
			usleep(breaks);
		}

		//Jedzenie
		begin = chrono::steady_clock::now();
		dur = chrono::steady_clock::now();

		while(chrono::duration_cast<chrono::duration<double> >(dur - begin).count() < eatTime && !end) {
			mutexWriter->lock();
			mvprintw(numb + 10, 0, "Klient %d: Je %d %%                ", numb, (int) round(
					chrono::duration_cast<chrono::duration<double> >(dur - begin).count() / eatTime * 100));
			mutexWriter->unlock();

			usleep(breaks);
			dur = chrono::steady_clock::now();
		}
		delete pizza;
		pizza = nullptr;

		mutexWriter->lock();
		mvprintw(10 + numb, 0, "Klient %d: Wstaje          ", numb);
		mutexWriter->unlock();

		while(haveChair && !end) {
			usleep(breaks);
			while(!mutexChairs->try_lock() && !end);

			for(int i = 0; i < chairsSize; i++) {
				if(!chairs[i]) {
					haveChair = false;
					chairs[i] = true;

					mutexWriter->lock();
					mvprintw(25, 50, "Krzesla");
					for(int j = 0; j < chairsSize; j++) {
						mvprintw(26, 50 + 2 * j, "%d", (int) chairs[j]);
					}
					mutexWriter->unlock();

					mutexChairs->unlock();
					break;
				}
			}
			if(!haveChair)
				break;
			mutexChairs->unlock();
		}
	}
}

void Client::threadClose() {
	end = true;
}

void Client::setPizza(Pizza *p) {
	pizza = p;
}
