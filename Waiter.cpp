#include <utility>

#include "Waiter.h"

Waiter::Waiter(int numb, mutex *mutexCountertop, Pizza **countertop, vector<Client *> clients, mutex *mutexWriter) :
		numb(numb), mutexCountertop(mutexCountertop), countertop(countertop), mutexWriter(mutexWriter) {
	this->clients = std::move(clients);
	threadW = new thread(&Waiter::threadWaiter, this);
}

Waiter::~Waiter() {
	threadW->join();
	delete threadW;
}

void Waiter::takePizza(int counterSize) {
	bool gotPizza = false;

	while(!gotPizza) {
		{
			unique_lock<mutex> lk_write(*mutexWriter);
			mvprintw(numb + 25, 0, "Kelner %d: Czeka na dostęp do blatu    ", numb);
		}

		{
			unique_lock<mutex> lk(*mutexCountertop);

			{
				unique_lock<mutex> lk_write(*mutexWriter);
				mvprintw(numb + 25, 0, "Kelner %d: Czeka na pizze do zaniesienia ", numb);
			}

			for(int i = 0; i < counterSize; i++) {
				if(countertop[i] != nullptr) {
					pizza = countertop[i];
					gotPizza = true;
					countertop[i] = nullptr;

					{
						unique_lock<mutex> lk_write(*mutexWriter);

						for(int j = 0; j < 20; j++) {
							mvprintw(21, 50 + 3 * j, "   ");
						}
						mvprintw(20, 50, "Blat");
						for(int j = 0; j < counterSize; j++) {
							if(countertop[j] != nullptr) {
								mvprintw(21, 50 + 3 * j, "%d", countertop[j]->getClient());
							}
						}
					}

					break;
				}
			}

		}

		if(gotPizza) {
			break;
		}

		usleep(breaks);
	}
}

void Waiter::deliverPizza() {
	float deliverTime = (rand() % 10) / 5. + 1;
	auto begin = chrono::steady_clock::now();
	auto dur = chrono::steady_clock::now();

	while(chrono::duration_cast<chrono::duration<double> >(dur - begin).count() < deliverTime && !end) {
		{
			unique_lock<mutex> lk_write(*mutexWriter);
			mvprintw(numb + 25, 0, "Kelner %d: Zanosi pizze klientowi %d%%            ", numb, (int) round(
					chrono::duration_cast<chrono::duration<double> >(dur - begin).count() / deliverTime * 100));
		}

		usleep(breaks);
		dur = chrono::steady_clock::now();
	}

	clients[pizza->getClient()]->setPizza(pizza);
	pizza = nullptr;
}


void Waiter::threadWaiter() {
	int counterSize = 10;

	while(!end) {
		//Podnosi pizze
		takePizza(counterSize);

		//Zanosi pizze
		deliverPizza();
	}
}

void Waiter::threadClose() {
	end = true;
}