#include "Client.h"

Client::Client() {
    end = false;
    breaks = 500000;
}

Client::Client(int numb, mutex *mutexChairs, bool *chairs, mutex *mutexOrdersList, vector<Order *> *ordersList) {
    this->numb = numb;
    this->mutexOrdersList = mutexOrdersList;
    this->ordersList = ordersList;
    this->mutexChairs = mutexChairs;
    this->chairs = chairs;

    end = false;
    breaks = 500000;
    threadC = new thread(&Client::threadClient, this);
}

Client::Client(const Client &orig) {
}

Client::~Client() {
    threadC->join();
    delete threadC;
}

void Client::print_queue(queue<Order *> q) {
    int i = 0;
    queue<Order *> *que = new queue<Order *>(q);
    while (!que->empty()) {
        mvprintw(1, 50 + 3 * i, "%d", que->front()->getClient());
        i++;
        que->pop();
    }
    delete que;

}

void Client::threadClient() {
    bool haveChair;
    vector<int> ingredients;
    chrono::_V2::steady_clock::time_point begin, dur;
    float eatTime;
    Order *order;

    int chairsSize = 10;

    while (!end) {
        haveChair = false;
        pizza = nullptr;
        eatTime = 1 + rand() % 3;

        usleep(breaks);

        //Siadanie
        while (!haveChair && !end) {
            mvprintw(10 + numb, 0, "Klient %d: Szuka miejsca do siedzenia\0", numb);
            usleep(breaks);
            while (!mutexChairs->try_lock() && !end);

            for (int i = 0; i < chairsSize; i++) {
                if (chairs[i]) {
                    haveChair = true;
                    chairs[i] = false;
                    mvprintw(25, 50, "Krzesla\0");
                    for (int i = 0; i < chairsSize; i++) {
                        mvprintw(26, 50 + 2 * i, "%d", (int) chairs[i]);
                    }
                    mutexChairs->unlock();
                    break;
                }
            }
            if (haveChair)
                break;
            mutexChairs->unlock();
        }

        //Składanie zamówienia

        mvprintw(10 + numb, 0, "Klient %d: Sklada zamowienie           \0", numb);
        usleep(breaks);
        while (!mutexOrdersList->try_lock() && !end);

        ingredients.push_back(0); //ciasto i inne składniki
        int numberOfIngredients = rand() % 4 + 3;
        for (int i = 1; i < numberOfIngredients; i++) {
            ingredients.push_back(rand() % 9 + 1);
        }

        order = new Order(numb, rand() % 2 + 1, ingredients);
        ordersList->push_back(order);

        ingredients.clear();
        for (int i = 0; i < 20; i++) {
            mvprintw(1, 50 + 3 * i, "   ");
        }
        mvprintw(0, 50, "Zamowienia\0");
        for (int i = 0; i < ordersList->size(); i++) {
            mvprintw(1, 50 + 3 * i, "%d", ordersList->operator[](i)->getClient());
        }

        mutexOrdersList->unlock();
        usleep(breaks);

        //Czekanie
        while (pizza == nullptr && !end) {
            mvprintw(10 + numb, 0, "Klient %d: Czeka na pizze           \0", numb);
            usleep(breaks);
        }

        //Jedzenie
        begin = chrono::steady_clock::now();
        dur = chrono::steady_clock::now();

        while (chrono::duration_cast<chrono::duration<double> >(dur - begin).count() < eatTime && !end) {
            mvprintw(numb + 10, 0, "Klient %d: Je %d %%                \0", numb, (int) round(
                    chrono::duration_cast<chrono::duration<double> >(dur - begin).count() / eatTime * 100));

            usleep(breaks);
            dur = chrono::steady_clock::now();
        }
        delete pizza;

        while (haveChair && !end) {
            mvprintw(10 + numb, 0, "Klient %d: Wstaje          \0", numb);
            usleep(breaks);
            while (!mutexChairs->try_lock() && !end);

            for (int i = 0; i < chairsSize; i++) {
                if (!chairs[i]) {
                    haveChair = false;
                    chairs[i] = true;
                    mvprintw(25, 50, "Krzesla\0");
                    for (int i = 0; i < chairsSize; i++) {
                        mvprintw(26, 50 + 2 * i, "%d", (int) chairs[i]);
                    }
                    mutexChairs->unlock();
                    break;
                }
            }
            if (!haveChair)
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
