//#include <utility>
#include "Client.h"
#include "Chair.h"

Client::Client(int numb, mutex *mutexChairs, vector<Chair *> *chairs,
			   mutex *mutexOrdersList, vector<Order *> *ordersList, condition_variable *cvClientMadeAnOrder,
			   mutex *mutexWriter,
			   vector<Client *> *otherClients) :
		numb(numb), mutexOrdersList(mutexOrdersList), ordersList(ordersList), cvClientMadeAnOrder(cvClientMadeAnOrder),
		mutexChairs(mutexChairs), chairs(chairs),
		mutexWriter(mutexWriter), otherClients(otherClients) {
	threadC = new thread(&Client::threadClient, this);
}

Client::~Client() {
	threadC->join();
	delete threadC;
}

void Client::printChairs() {
	unique_lock<mutex> lk_write(*mutexWriter);

	mvprintw(25, 50, "Krzesla typu filozof");
	for(int j = 0; j < chairs->size(); j++) {
		if(chairs->at(j)->getClientType() != -1) {
			if(has_colors()) {
				auto _color = chairs->at(j)->getClientType() ? 3 : 4;

				attron(COLOR_PAIR(_color));
				mvprintw(26, 50 + 3 * j, "%2d", chairs->at(j)->getClientNumber());
				attroff(COLOR_PAIR(_color));
			} else {
				mvprintw(26, 50 + 3 * j, "%2d", chairs->at(j)->getClientNumber());
			}
		} else {
			mvprintw(26, 50 + 3 * j, "__");
		}
	}
}

void Client::takeASeat() {
	bool hasChair = false;

	chrono::steady_clock::time_point begin = chrono::steady_clock::now(), dur;
	while(!hasChair && !end) {
		dur = chrono::steady_clock::now();
		auto t = chrono::duration_cast<chrono::seconds>(dur - begin).count();

		auto color = clientType ? 3 : 4;

		{
			unique_lock<mutex> lk_write(*mutexWriter);
			if(has_colors()) {
				attron(COLOR_PAIR(color));
				mvprintw(10 + numb, 0, "Klient %2d", numb);
				attroff(COLOR_PAIR(color));
			} else {
				mvprintw(10 + numb, 0, "Klient %2d", numb);
			}
			mvprintw(10 + numb, 9, ": Szuka miejsca do siedzenia [%d s]    ", t);
		}

		usleep(breaks);

		{
			unique_lock<mutex> lk(*mutexChairs);

			for(int i = 0; i < chairs->size(); i++) {
				if(chairs->at(i)->isFree()) {
					auto right = (i + 1) % chairs->size(), left = (i - 1 != -1 ? i - 1 : chairs->size() - 1);
					auto leftChair = chairs->at(left);
					auto rightChair = chairs->at(right);

					if(!leftChair->isFree() || !rightChair->isFree()) {
						if((leftChair->getClientType() != clientType && leftChair->getClientType() != -1)
						   || (rightChair->getClientType() != clientType && rightChair->getClientType() != -1)) {
							continue;
						}
					}

					hasChair = true;
					chairs->at(i)->setClient(this);
					chairTaken = i;

					printChairs();

					break;
				}
			}
		}

		if(hasChair) {
			break;
		} else {
			// Randomly decide whether client stays or leaves
			// 1% chance that they leave
			if(random() % 100 == 42) {
				oughtToLeave = true;
				break;
			}
		}
	}
}

void Client::makeOrder() {
	{
		unique_lock<mutex> lk_write(*mutexWriter);
		mvprintw(10 + numb, 11, "Sklada zamowienie                ");
	}

	usleep(breaks);

	vector<int> ingredients;

	{
		unique_lock<mutex> lk(*mutexOrdersList);

		ingredients.push_back(0); // Składniki
		int numberOfIngredients = random() % 4 + 3;
		for(int i = 1; i < numberOfIngredients; i++) {
			ingredients.push_back(random() % 9 + 1);
		}

		auto order = new Order(numb, random() % 2 + 1, ingredients);
		ordersList->push_back(order);

		ingredients.clear();

		{
			unique_lock<mutex> lk_write(*mutexWriter);

			for(int i = 0; i < 20; i++) {
				mvprintw(1, 50 + 3 * i, "   ");
			}
			mvprintw(0, 50, "Zamowienia");
			for(int i = 0; i < ordersList->size(); i++) {
				mvprintw(1, 50 + 3 * i, "%d", ordersList->operator[](i)->getClient());
			}
		}
	}
	cvClientMadeAnOrder->notify_one();
}

void Client::waitForDelivery() {
	chrono::steady_clock::time_point begin = chrono::steady_clock::now(), dur;

	while(pizza == nullptr && !end) {
		dur = chrono::steady_clock::now();

		{
			unique_lock<mutex> lk_write(*mutexWriter);
			mvprintw(10 + numb, 11, "Czeka na pizze [%d s]          ",
					 chrono::duration_cast<chrono::seconds>(dur - begin).count());
		}

		usleep(breaks);
	}
}

void Client::eat() {
	float eatTime = 1 + random() % 3;

	auto begin = chrono::steady_clock::now();
	auto dur = chrono::steady_clock::now();

	while(chrono::duration_cast<chrono::duration<double> >(dur - begin).count() < eatTime && !end) {
		{
			unique_lock<mutex> lk_write(*mutexWriter);
			mvprintw(numb + 10, 11, "Je [%d %%]                 ", (int) round(
					chrono::duration_cast<chrono::duration<double> >(dur - begin).count() / eatTime * 100));
		}

		usleep(breaks);
		dur = chrono::steady_clock::now();
	}

	delete pizza;
	pizza = nullptr;
}

void Client::leave() {
	{
		unique_lock<mutex> lk_write(*mutexWriter);
		mvprintw(10 + numb, 11, "Wstaje                     ");
	}

	usleep(breaks);

	{
		unique_lock<mutex> lk(*mutexChairs);

		chairs->at(chairTaken)->noClient();
		chairTaken = -1;

		printChairs();
	}
}

void Client::leaveImmediately() {
	{
		unique_lock<mutex> lk_write(*mutexWriter);
		mvprintw(10 + numb, 11, "                                 ");
		if(has_colors()) {
			attron(COLOR_PAIR(2));
			mvprintw(10 + numb, 11, "WYCHODZI Z HUKIEM");
			attroff(COLOR_PAIR(2));
		} else {
			mvprintw(10 + numb, 11, "WYCHODZI Z HUKIEM");
		}
	}

	usleep(3 * breaks);

	{
		unique_lock<mutex> lk_write(*mutexWriter);
		mvprintw(10 + numb, 0, "Klient %2d: ...                              ", numb);
	}

	usleep(10 * breaks);

	oughtToLeave = false;
}

void Client::threadClient() {
	while(!end) {
		clientType = random() % 2; // Randomize client's type

		usleep(breaks);

		// Zajęcie miejsca
		takeASeat();

		// Jeśli klient się zdenerwował, opuszcza lokal w trybie now.
		if(oughtToLeave) {
			leaveImmediately();
			continue;
		}

		// Składanie zamówienia
		makeOrder();

		// Oczekiwanie
		waitForDelivery();

		// Posiłek
		eat();

		// Wychodzenie
		leave();
	}
}

void Client::threadClose() {
	end = true;
}

void Client::setPizza(Pizza *p) {
	pizza = p;
}
