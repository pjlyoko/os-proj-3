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

void Waiter::threadWaiter() {
	bool gotPizza;
	float deliverTime;
	chrono::_V2::steady_clock::time_point begin, dur;
	int counterSize = 10;

	while(!end) {
		gotPizza = false;
		deliverTime = (rand() % 10) / 5 + 1;

		//Podnosi pizze
		while(!gotPizza) {

			mutexWriter->lock();
			mvprintw(numb + 25, 0, "Kelner %d: Czeka na dostęp do blatu    ", numb);
			mutexWriter->unlock();
			while(!mutexCountertop->try_lock() && !end);

			mutexWriter->lock();
			mvprintw(numb + 25, 0, "Kelner %d: Czeka na pizze do zaniesienia ", numb);
			mutexWriter->unlock();

			for(int i = 0; i < counterSize; i++) {
				if(countertop[i] != nullptr) {
					pizza = countertop[i];
					gotPizza = true;
					countertop[i] = nullptr;

					mutexWriter->lock();
					for(int i = 0; i < 20; i++) {
						mvprintw(21, 50 + 3 * i, "   ");
					}
					mvprintw(20, 50, "Blat");
					for(int i = 0; i < counterSize; i++) {
						if(countertop[i] != nullptr) {
							mvprintw(21, 50 + 3 * i, "%d", countertop[i]->getClient());
						}
					}
					mutexWriter->unlock();

					mutexCountertop->unlock();
					break;
				}
			}
			if(gotPizza)
				break;

			mutexCountertop->unlock();
			usleep(breaks);
		}

		//Zanosi pizze
		begin = chrono::steady_clock::now();
		dur = chrono::steady_clock::now();

		while(chrono::duration_cast<chrono::duration<double> >(dur - begin).count() < deliverTime && !end) {
			mutexWriter->lock();
			mvprintw(numb + 25, 0, "Kelner %d: Zanosi pizze klientowi %d %%           ", numb, (int) round(
					chrono::duration_cast<chrono::duration<double> >(dur - begin).count() / deliverTime * 100));
			mutexWriter->unlock();

			usleep(breaks);
			dur = chrono::steady_clock::now();
		}
		clients[pizza->getClient()]->setPizza(pizza);
		pizza = nullptr;
	}
}

void Waiter::threadClose() {
	end = true;
}