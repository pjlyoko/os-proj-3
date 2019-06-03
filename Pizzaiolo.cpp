#include "Pizzaiolo.h"

Pizzaiolo::Pizzaiolo(int numb, mutex *mutexOrdersList, vector<Order *> *ordersList, mutex *mutexFridge, int *fridge,
					 mutex *mutexTools, bool *tools, mutex *mutexFurnaces, int *furnace, mutex *mutexCountertop,
					 Pizza **countertop, mutex *mutexWriter) {
	this->numb = numb;
	this->mutexOrdersList = mutexOrdersList;
	this->ordersList = ordersList;
	this->mutexFridge = mutexFridge;
	this->fridge = fridge;
	this->mutexTools = mutexTools;
	this->tools = tools;
	this->mutexFurnaces = mutexFurnaces;
	this->furnaces = furnace;
	this->mutexCountertop = mutexCountertop;
	this->countertop = countertop;
	this->mutexWriter = mutexWriter;

	threadC = new thread(&Pizzaiolo::threadStart, this);
}

Pizzaiolo::Pizzaiolo(const Pizzaiolo &orig) {
}

Pizzaiolo::~Pizzaiolo() {
	threadC->join();
	delete threadC;
}

void Pizzaiolo::threadStart() {

	bool allIngredients, haveTools, haveFurnace, havePlace;
	float preparingTime = 3;
	float bakeTime = 5;
	int size; //1 - mala pizza, 2 - duza
	int client;
	chrono::_V2::steady_clock::time_point begin, dur;
	Order *order;

	int fridgeSize = 10, counterSize = 10, toolsSize = 4, furnancesSize = 5;

	while(!end) {

		allIngredients = false;
		haveTools = false;
		haveFurnace = false;
		havePlace = false;

		//odbieranie zamównienia
		mutexWriter->lock();
		mvprintw(numb, 0, "Kucharz %d: Czyta liste zamowien          ", numb);
		mutexWriter->unlock();
		while(!mutexOrdersList->try_lock() && !end);

		if(!ordersList->empty()) {
			order = ordersList->front();
			// usuwa zamówienie z listy zamówień
			ordersList->erase(remove(ordersList->begin(), ordersList->end(), order));

			for(int i = 0; i < order->getIngredients().size(); i++) {
				ingredients.push_back(order->getIngredients()[i]);
			}

			client = order->getClient();
			size = order->getSize();

			delete order;
			order = nullptr;

			mutexWriter->lock();
			mvprintw(0, 50, "Zamowienia");
			for(int i = 0; i < 20; i++) {
				mvprintw(1, 50 + 3 * i, "   ");
			}
			for(int i = 0; i < ordersList->size(); i++) {
				mvprintw(1, 50 + 3 * i, "%d", ordersList->operator[](i)->getClient());
			}
			mutexWriter->unlock();

			mutexOrdersList->unlock();

			//zbieranie składników
			mutexWriter->lock();
			mvprintw(numb, 0, "Kucharz %d: Wyjmuje skladniki z lodowki          ", numb);
			mutexWriter->unlock();
			{
				unique_lock<mutex> lk(*mutexFridge);

				for(int ingredient : ingredients) {
					fridge[ingredient]++;
				}

				mutexWriter->lock();
				mvprintw(5, 50, "Lodowka");
				for(int i = 0; i < fridgeSize; i++) {
					mvprintw(6, 50 + 5 * i, "%d", fridge[i]);
				}
				mutexWriter->unlock();

				usleep(breaks);
			}
			ingredients.clear();

			//podnoszenie narzedzi
			mutexWriter->lock();
			mvprintw(numb, 0, "Kucharz %d: podnosi narzedzia               ", numb);
			mutexWriter->unlock();

			while(!haveTools && !end) {
				while(!mutexTools->try_lock() && !end);

				for(int i = 0; i < toolsSize; i++) {
					if(tools[i]) {
						haveTools = true;
						tools[i] = false;

						mutexWriter->lock();
						mvprintw(10, 50, "Narzedzia");
						for(int i = 0; i < toolsSize; i++) {
							mvprintw(11, 50 + 3 * i, "%d", (int) tools[i]);
						}
						mutexWriter->unlock();

						mutexTools->unlock();
						break;
					}
				}
				if(haveTools)
					break;
				else
					mutexTools->unlock();

				usleep(breaks);
			}

			//Przygotowywanie pizzy
			begin = chrono::steady_clock::now();
			dur = chrono::steady_clock::now();

			while(chrono::duration_cast<chrono::duration<double> >(dur - begin).count() < preparingTime && !end) {
				mutexWriter->lock();
				mvprintw(numb, 0, "Kucharz %d: Przygotowuje pizze %d %%            ", numb, (int) round(
						chrono::duration_cast<chrono::duration<double> >(dur - begin).count() / preparingTime * 100));
				mutexWriter->unlock();

				usleep(breaks);
				dur = chrono::steady_clock::now();
			}

			//Odlożenie narzędzi
			while(haveTools && !end) {
				mutexWriter->lock();
				mvprintw(numb, 0, "Kucharz %d: Odklada narzedzia               ", numb);
				mutexWriter->unlock();

				usleep(breaks);
				while(!mutexTools->try_lock() && !end);

				for(int i = 0; i < toolsSize; i++) {
					if(!tools[i]) {
						haveTools = false;
						tools[i] = true;

						mutexWriter->lock();
						mvprintw(10, 50, "Narzedzia");
						for(int i = 0; i < toolsSize; i++) {
							mvprintw(11, 50 + 3 * i, "%d", (int) tools[i]);
						}
						mutexWriter->unlock();

						mutexTools->unlock();
						break;
					}
				}
				if(!haveTools)
					break;
				else
					mutexTools->unlock();

				usleep(breaks);
			}

			//Pieczenie pizzy
			while(!haveFurnace && !end) {
				mutexWriter->lock();
				mvprintw(numb, 0, "Kucharz %d: Wklada pizze do pieca          ", numb);
				mutexWriter->unlock();

				while(!mutexFurnaces->try_lock() && !end);

				for(int i = 0; i < furnancesSize; i++) {
					if(furnaces[i] >= size) {
						haveFurnace = true;
						furnaces[i] -= size;

						mutexWriter->lock();
						mvprintw(15, 50, "Piece");
						for(int i = 0; i < furnancesSize; i++) {
							mvprintw(16, 50 + 3 * i, "%d", furnaces[i]);
						}
						mutexWriter->unlock();

						mutexFurnaces->unlock();
						break;
					}
				}
				if(haveFurnace)
					break;
				else
					mutexFurnaces->unlock();

				usleep(breaks);
			}

			begin = chrono::steady_clock::now();
			dur = chrono::steady_clock::now();

			while(chrono::duration_cast<chrono::duration<double> >(dur - begin).count() < bakeTime && !end) {
				mutexWriter->lock();
				mvprintw(numb, 0, "Kucharz %d: Piecze pizze %d %%           ", numb, (int) round(
						chrono::duration_cast<chrono::duration<double> >(dur - begin).count() / bakeTime * 100));
				mutexWriter->unlock();

				usleep(breaks);
				dur = chrono::steady_clock::now();
			}

			mutexWriter->lock();
			mvprintw(numb, 0, "Kucharz %d: Wyjmuje pizze z pieca          ", numb);
			mutexWriter->unlock();

			while(haveFurnace && !end) {
				usleep(breaks);
				while(!mutexFurnaces->try_lock() && !end);

				for(int i = 0; i < furnancesSize; i++) {
					if(furnaces[i] <= 2 - size) {
						haveFurnace = false;
						furnaces[i] += size;

						mutexWriter->lock();
						mvprintw(15, 50, "Piece");
						for(int i = 0; i < furnancesSize; i++) {
							mvprintw(16, 50 + 3 * i, "%d", furnaces[i]);
						}
						mutexWriter->unlock();

						mutexFurnaces->unlock();
						break;
					}
				}
				if(!haveFurnace)
					break;
				else
					mutexFurnaces->unlock();
			}

			//Odłozenie na blat
			mutexWriter->lock();
			mvprintw(numb, 0, "Kucharz %d: Odklada pizze na blat          ", numb);
			mutexWriter->unlock();
			while(!havePlace && !end) {
				usleep(breaks);
				while(!mutexCountertop->try_lock() && !end);

				for(int i = 0; i < counterSize; i++) {
					if(countertop[i] == nullptr) {
						havePlace = true;
						countertop[i] = new Pizza(client);

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
				if(havePlace)
					break;
				else
					mutexCountertop->unlock();
			}

		} else {
			mutexOrdersList->unlock();

			mutexWriter->lock();
			mvprintw(numb, 0, "Kucharz %d: Czeka na zamowienia          ", numb);
			mutexWriter->unlock();

			usleep(breaks);
		}
	}
}

void Pizzaiolo::threadClose() {
	end = true;
}