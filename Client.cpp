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

    chrono::steady_clock::time_point begin = chrono::steady_clock::now(), dur;
    while(!hasChair && !end) {
	    dur = chrono::steady_clock::now();
	    auto t = chrono::duration_cast<chrono::seconds>(dur - begin).count();

		{
			unique_lock<mutex> lk_write(*mutexWriter);
			mvprintw(10 + numb, 0, "Klient %2d: Szuka miejsca do siedzenia [%d s]", numb, t);
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
		mvprintw(10 + numb, 0, "Klient %2d: Sklada zamowienie                ", numb);
	}

	usleep(breaks);

	vector<int> ingredients;

	{
		unique_lock<mutex> lk(*mutexOrdersList);

		ingredients.push_back(0); //ciasto i inne składniki
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
}

void Client::waitForDelivery() {
	chrono::steady_clock::time_point begin = chrono::steady_clock::now(), dur;

	while(pizza == nullptr && !end) {
	    dur = chrono::steady_clock::now();

        {
            unique_lock<mutex> lk_write(*mutexWriter);
            mvprintw(10 + numb, 0, "Klient %2d: Czeka na pizze [%d s]          ", numb,
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
			mvprintw(numb + 10, 0, "Klient %2d: Je %d %%                ", numb, (int) round(
					chrono::duration_cast<chrono::duration<double> >(dur - begin).count() / eatTime * 100));
		}

		usleep(breaks);
		dur = chrono::steady_clock::now();
	}

	delete pizza;
	pizza = nullptr;
}

void Client::leave(int chairsSize) {
	{
		unique_lock<mutex> lk_write(*mutexWriter);
		mvprintw(10 + numb, 0, "Klient %2d: Wstaje          ", numb);
	}

	usleep(breaks);

	{
		unique_lock<mutex> lk(*mutexChairs);

		chairs[chairTaken] = true;
		chairTaken = -1;

		{
			unique_lock<mutex> lk_write(*mutexWriter);
			mvprintw(25, 50, "Krzesla");
			for(int j = 0; j < chairsSize; j++) {
				mvprintw(26, 50 + 2 * j, "%d", (int) chairs[j]);
			}
		}
	}
}

void Client::leaveImmediately() {
    {
        unique_lock<mutex> lk_write(*mutexWriter);
        mvprintw(10 + numb, 0, "Klient %2d:                                 ", numb);
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
        mvprintw(10 + numb, 0, "Klient %2d: ...                         ", numb);
    }

    usleep(10 * breaks);

    oughtToLeave = false;
}

void Client::threadClient() {
	int chairsSize = 10;

	while(!end) {
		usleep(breaks);

		//Siadanie
		takeASeat(chairsSize);

		if(oughtToLeave) {
		    leaveImmediately();
		    continue;
		}

		//Składanie zamówienia
		makeOrder();

		//Czekanie
		waitForDelivery();

		//Jedzenie
		eat();

		// Wychodzenie
		leave(chairsSize);
	}
}

void Client::threadClose() {
	end = true;
}

void Client::setPizza(Pizza *p) {
	pizza = p;
}
