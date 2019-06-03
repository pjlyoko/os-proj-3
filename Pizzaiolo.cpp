#include "Pizzaiolo.h"

Pizzaiolo::Pizzaiolo(int numb, mutex *mutexOrdersList, vector<Order *> *ordersList, mutex *mutexFridge, int *fridge,
					 mutex *mutexTools, bool *tools, mutex *mutexFurnaces, int *furnace, mutex *mutexCountertop,
					 Pizza **countertop, mutex *mutexWriter) : numb(numb), mutexOrdersList(mutexOrdersList),
															   ordersList(ordersList), mutexFridge(mutexFridge),
															   fridge(fridge), mutexTools(mutexTools),
															   tools(tools), mutexFurnaces(mutexFurnaces),
															   furnaces(furnace), mutexCountertop(mutexCountertop),
															   countertop(countertop), mutexWriter(mutexWriter) {
	threadC = new thread(&Pizzaiolo::threadStart, this);
}

Pizzaiolo::~Pizzaiolo() {
	threadC->join();
	delete threadC;
}

void Pizzaiolo::takeIngredients(int fridgeSize) {
	{
		unique_lock<mutex> lk_write(*mutexWriter);
		mvprintw(numb, 0, "Kucharz %d: Wyjmuje skladniki z lodowki          ", numb);
	}

	{
		unique_lock<mutex> lk(*mutexFridge);

		for(int ingredient : ingredients) {
			fridge[ingredient]++;
		}
	}

	{
		unique_lock<mutex> lk_write(*mutexWriter);
		mvprintw(5, 50, "Lodowka");
		for(int i = 0; i < fridgeSize; i++) {
			mvprintw(6, 50 + 5 * i, "%d", fridge[i]);
		}
	}

	usleep(breaks);

	ingredients.clear();
}

void Pizzaiolo::takeTool(int toolsSize) {
	{
		unique_lock<mutex> lk_write(*mutexWriter);
		mvprintw(numb, 0, "Kucharz %d: Podnosi narzedzie               ", numb);
	}

	bool hasTool = false;

	while(!end) {
		{
			unique_lock<mutex> lk(*mutexTools);

			for(int i = 0; i < toolsSize; i++) {
				if(tools[i]) {
					tools[i] = false;
					hasTool = true;

					toolTaken = i;

					{
						unique_lock<mutex> lk_write(*mutexWriter);
						mvprintw(10, 50, "Narzedzia");
						for(int j = 0; j < toolsSize; j++) {
							mvprintw(11, 50 + 3 * j, "%d", (int) tools[j]);
						}
					}

					break;
				}
			}
		}

		if(hasTool) {
			break;
		}

		usleep(breaks);
	}
}

void Pizzaiolo::preparePizza(float preparingTime) {
	auto begin = chrono::steady_clock::now();
	auto dur = chrono::steady_clock::now();

	while(chrono::duration_cast<chrono::duration<double> >(dur - begin).count() < preparingTime && !end) {
		{
			unique_lock<mutex> lk_write(*mutexWriter);
			mvprintw(numb, 0, "Kucharz %d: Przygotowuje pizze %d %%            ", numb, (int) round(
					chrono::duration_cast<chrono::duration<double> >(dur - begin).count() / preparingTime * 100));
		}

		usleep(breaks);
		dur = chrono::steady_clock::now();
	}
}

void Pizzaiolo::returnTool(int toolsSize) {
	{
		unique_lock<mutex> lk_write(*mutexWriter);
		mvprintw(numb, 0, "Kucharz %d: Odklada narzedzia               ", numb);
	}

	usleep(breaks);

	mutexTools->lock();
	//while(!mutexTools->try_lock() && !end);

	tools[toolTaken] = true;
	toolTaken = -1;

	mutexTools->unlock();

	{
		unique_lock<mutex> lk_write(*mutexWriter);
		mvprintw(10, 50, "Narzedzia");
		for(int j = 0; j < toolsSize; j++) {
			mvprintw(11, 50 + 3 * j, "%d", (int) tools[j]);
		}
	}
}

void Pizzaiolo::bakePizza(int pizzaSize, float bakeTime, int furnacesSize) {
	{
		unique_lock<mutex> lk_write(*mutexWriter);
		mvprintw(numb, 0, "Kucharz %d: Wklada pizze do pieca          ", numb);
	}

	int haveFurnace = false;

	while(!haveFurnace) {
		mutexFurnaces->lock();

		for(int i = 0; i < furnacesSize; i++) {
			if(furnaces[i] >= pizzaSize) {
				haveFurnace = true;
				furnaces[i] -= pizzaSize;
				furnaceUsed = i;

				{
					unique_lock<mutex> lk_write(*mutexWriter);
					mvprintw(15, 50, "Piece");
					for(int j = 0; j < furnacesSize; j++) {
						mvprintw(16, 50 + 3 * j, "%d", furnaces[j]);
					}
				}

				mutexFurnaces->unlock();
				break;
			}
		}

		mutexFurnaces->unlock();

		if(haveFurnace) {
			break;
		} else {
			usleep(breaks);
		}
	}

	auto begin = chrono::steady_clock::now();
	auto dur = chrono::steady_clock::now();

	while(chrono::duration_cast<chrono::duration<double> >(dur - begin).count() < bakeTime && !end) {
		{
			unique_lock<mutex> lk_write(*mutexWriter);
			mvprintw(numb, 0, "Kucharz %d: Piecze pizze %d%%            ", numb, (int) round(
					chrono::duration_cast<chrono::duration<double> >(dur - begin).count() / bakeTime * 100));
		}

		usleep(breaks);
		dur = chrono::steady_clock::now();
	}
}

void Pizzaiolo::takePizzaFromFurnace(int pizzaSize, int furnacesSize) {
	{
		unique_lock<mutex> lk_write(*mutexWriter);
		mvprintw(numb, 0, "Kucharz %d: Wyjmuje pizze z pieca          ", numb);
	}

	usleep(breaks);

	unique_lock<mutex> lk(*mutexFurnaces);

	furnaces[furnaceUsed] += pizzaSize;
	furnaceUsed = -1;

	{
		unique_lock<mutex> lk_write(*mutexWriter);
		mvprintw(15, 50, "Piece");
		for(int j = 0; j < furnacesSize; j++) {
			mvprintw(16, 50 + 3 * j, "%d", furnaces[j]);
		}
	}
}

void Pizzaiolo::putPizzaOnCountertop(int client, int countertopSize) {
	{
		unique_lock<mutex> lk_write(*mutexWriter);
		mvprintw(numb, 0, "Kucharz %d: Odklada pizze na blat          ", numb);
	}

	bool hasPlace = false;

	usleep(breaks);

	while(!hasPlace) {
		{
			unique_lock<mutex> lk(*mutexCountertop);
			for(int i = 0; i < countertopSize; i++) {
				if(countertop[i] == nullptr) {
					hasPlace = true;
					countertop[i] = new Pizza(client);

					{
						unique_lock<mutex> lk_write(*mutexWriter);

						for(int j = 0; j < 20; j++) {
							mvprintw(21, 50 + 3 * j, "   ");
						}
						mvprintw(20, 50, "Blat");
						for(int j = 0; j < countertopSize; j++) {
							if(countertop[j] != nullptr) {
								mvprintw(21, 50 + 3 * j, "%d", countertop[j]->getClient());
							}
						}
					}

					break;
				}
			}
		}

		if(hasPlace) {
			break;
		} else {
			usleep(breaks);
		}
	}
}

void Pizzaiolo::threadStart() {
	float preparingTime = 3;
	float bakeTime = 5;
	int pizzaSize; //1 - mala, 2 - duza
	int client;
	chrono::_V2::steady_clock::time_point begin, dur;
	Order *order;

	int fridgeSize = 10, counterSize = 10, toolsSize = 4, furnacesSize = 5;

	while(!end) {
		//odbieranie zamównienia
		{
			unique_lock<mutex> lk_write(*mutexWriter);
			mvprintw(numb, 0, "Kucharz %d: Czyta liste zamowien          ", numb);
		}

		mutexOrdersList->lock();

		if(ordersList->empty()) {
			mutexOrdersList->unlock();

			{
				unique_lock<mutex> lk_write(*mutexWriter);
				mvprintw(numb, 0, "Kucharz %d: Czeka na zamowienia          ", numb);
			}

			usleep(breaks);
		} else {
			order = ordersList->front();
			// usuwa zamówienie z listy zamówień
			ordersList->erase(remove(ordersList->begin(), ordersList->end(), order));

			for(int i = 0; i < order->getIngredients().size(); i++) {
				ingredients.push_back(order->getIngredients()[i]);
			}

			client = order->getClient();
			pizzaSize = order->getSize();

			delete order;
			order = nullptr;

			{
				unique_lock<mutex> lk_write(*mutexWriter);

				mvprintw(0, 50, "Zamowienia oczekujace");
				for(int i = 0; i < 20; i++) {
					mvprintw(1, 50 + 3 * i, "   ");
				}
				for(int i = 0; i < ordersList->size(); i++) {
					mvprintw(1, 50 + 3 * i, "%d", ordersList->operator[](i)->getClient());
				}
			}

			mutexOrdersList->unlock();

			//zbieranie składników
			takeIngredients(fridgeSize);

			//podnoszenie narzedzi
			takeTool(toolsSize);

			//Przygotowywanie pizzy
			preparePizza(preparingTime);

			//Odlożenie narzędzi
			returnTool(toolsSize);

			//Pieczenie pizzy
			bakePizza(pizzaSize, bakeTime, furnacesSize);

			takePizzaFromFurnace(pizzaSize, furnacesSize);

			//Odłozenie na blat
			putPizzaOnCountertop(client, counterSize);
		}
	}
}

void Pizzaiolo::threadClose() {
	end = true;
}