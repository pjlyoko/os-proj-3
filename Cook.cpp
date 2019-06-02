#include "Cook.h"

Cook::Cook() {
    end = false;
    breaks = 500000;

}

Cook::Cook(int numb, mutex *mutexOrdersList, vector<Order *> *ordersList, mutex *mutexFridge, int *fridge,
           mutex *mutexTools, bool *tools, mutex *mutexFurnances, int *furnace, mutex *mutexCountertop,
           Pizza **countertop) {
    this->numb = numb;
    this->mutexOrdersList = mutexOrdersList;
    this->ordersList = ordersList;
    this->mutexFridge = mutexFridge;
    this->fridge = fridge;
    this->mutexTools = mutexTools;
    this->tools = tools;
    this->mutexFurnances = mutexFurnances;
    this->furnances = furnace;
    this->mutexCountertop = mutexCountertop;
    this->countertop = countertop;

    end = false;
    breaks = 500000;
    threadC = new thread(&Cook::threadCook, this);
}

Cook::Cook(const Cook &orig) {
}

Cook::~Cook() {
    threadC->join();
    delete threadC;
}

void Cook::threadCook() {

    bool allIngredients, haveTools, haveFurnance, havePlace;
    float preparingTime = 3;
    float bakeTime = 5;
    int size; //1 - mala pizza, 2 - duza
    int client;
    chrono::_V2::steady_clock::time_point begin, dur;
    Order *order;

    int fridgeSize = 10, counterSize = 10, toolsSize = 4, furnancesSize = 5;

    while (!end) {

        allIngredients = false;
        haveTools = false;
        haveFurnance = false;
        havePlace = false;

        //odbieranie zamównienia
        while (!mutexOrdersList->try_lock() && !end) {
            mvprintw(numb, 0, "Kucharz %d: Czyta liste zamowien          \0", numb);
        }

        if (!ordersList->empty()) {

            order = ordersList->front();

            for (int i = 0; i < order->getIngredients().size(); i++) {
                ingredients.push_back(order->getIngredients()[i]);
            }

            client = order->getClient();
            size = order->getSize();
            order = nullptr;
            ordersList->erase(ordersList->begin());

            mvprintw(0, 50, "Zamowienia\0");
            for (int i = 0; i < 20; i++)
                mvprintw(1, 50 + 3 * i, "   ");
            for (int i = 0; i < ordersList->size(); i++)
                mvprintw(1, 50 + 3 * i, "%d", ordersList->operator[](i)->getClient());

            mutexOrdersList->unlock();

            //zbieranie składników
            while (!allIngredients && !end) {
                int i;
                mvprintw(numb, 0, "Kucharz %d: Wyjmuje skladniki z lodowki          \0", numb);
                while (!mutexFridge->try_lock() && !end);

                mvprintw(5, 50, "Lodowka\0");
                for (int i = 0; i < fridgeSize; i++) {
                    mvprintw(6, 50 + 3 * i, "%d", fridge[i]);
                }

                for (i = 0; i < ingredients.size(); i++) {
                    if (fridge[ingredients[i]] <= 0) {
                        mvprintw(numb, 0, "Kucharz %d: Brak skladnikow na pizze          \0", numb);
                        mutexFridge->unlock();
                        break;
                    }
                }
                if (i >= ingredients.size()) {
                    allIngredients = true;
                    for (i = 0; i < ingredients.size(); i++) {
                        fridge[ingredients[i]]--;
                    }
                    mvprintw(5, 50, "Lodowka\0");
                    for (int i = 0; i < fridgeSize; i++) {
                        mvprintw(6, 50 + 3 * i, "%d ", fridge[i]);
                    }

                    mutexFridge->unlock();
                    break;
                }

                usleep(breaks);
            }
            ingredients.clear();
            //podnoszenie narzedzi
            while (!haveTools && !end) {
                mvprintw(numb, 0, "Kucharz %d: podnosi narzedzia               \0", numb);
                while (!mutexTools->try_lock() && !end);

                for (int i = 0; i < toolsSize; i++) {
                    if (tools[i]) {
                        haveTools = true;
                        tools[i] = false;
                        mvprintw(10, 50, "Narzedzia\0");
                        for (int i = 0; i < toolsSize; i++) {
                            mvprintw(11, 50 + 3 * i, "%d", (int) tools[i]);
                        }
                        mutexTools->unlock();
                        break;
                    }
                }
                if (haveTools)
                    break;
                else
                    mutexTools->unlock();

                usleep(breaks);
            }

            //Przygotowywanie pizzy
            begin = chrono::steady_clock::now();
            dur = chrono::steady_clock::now();

            while (chrono::duration_cast<chrono::duration<double> >(dur - begin).count() < preparingTime && !end) {
                mvprintw(numb, 0, "Kucharz %d: Przygotowuje pizze %d %%            \0", numb, (int) round(
                        chrono::duration_cast<chrono::duration<double> >(dur - begin).count() / preparingTime * 100));

                usleep(breaks);
                dur = chrono::steady_clock::now();
            }

            //Odlożenie narzędzi
            while (haveTools && !end) {
                mvprintw(numb, 0, "Kucharz %d: Odklada narzedzia               \0", numb);
                usleep(breaks);
                while (!mutexTools->try_lock() && !end);

                for (int i = 0; i < toolsSize; i++) {
                    if (!tools[i]) {
                        haveTools = false;
                        tools[i] = true;
                        mvprintw(10, 50, "Narzedzia\0");
                        for (int i = 0; i < toolsSize; i++) {
                            mvprintw(11, 50 + 3 * i, "%d", (int) tools[i]);
                        }
                        mutexTools->unlock();
                        break;
                    }
                }
                if (!haveTools)
                    break;
                else
                    mutexTools->unlock();

                usleep(breaks);
            }

            //Pieczenie pizzy
            while (!haveFurnance && !end) {
                while (!mutexFurnances->try_lock() && !end) {
                    mvprintw(numb, 0, "Kucharz %d: Wklada pizze do pieca          \0", numb);
                }
                for (int i = 0; i < furnancesSize; i++) {
                    if (furnances[i] >= size) {
                        haveFurnance = true;
                        furnances[i] -= size;
                        mvprintw(15, 50, "Piece\0");
                        for (int i = 0; i < furnancesSize; i++) {
                            mvprintw(16, 50 + 3 * i, "%d", furnances[i]);
                        }
                        mutexFurnances->unlock();
                        break;
                    }
                }
                if (haveFurnance)
                    break;
                else
                    mutexFurnances->unlock();

                usleep(breaks);
            }

            begin = chrono::steady_clock::now();
            dur = chrono::steady_clock::now();

            while (chrono::duration_cast<chrono::duration<double> >(dur - begin).count() < bakeTime && !end) {
                mvprintw(numb, 0, "Kucharz %d: Piecze pizze %d %%           \0", numb, (int) round(
                        chrono::duration_cast<chrono::duration<double> >(dur - begin).count() / bakeTime * 100));

                usleep(breaks);
                dur = chrono::steady_clock::now();
            }

            while (haveFurnance && !end) {
                mvprintw(numb, 0, "Kucharz %d: Wyjmuje pizze z pieca          \0", numb);
                usleep(breaks);
                while (!mutexFurnances->try_lock() && !end);

                for (int i = 0; i < furnancesSize; i++) {
                    if (furnances[i] <= 2 - size) {
                        haveFurnance = false;
                        furnances[i] += size;
                        mvprintw(15, 50, "Piece\0");
                        for (int i = 0; i < furnancesSize; i++) {
                            mvprintw(16, 50 + 3 * i, "%d", furnances[i]);
                        }
                        mutexFurnances->unlock();
                        break;
                    }
                }
                if (!haveFurnance)
                    break;
                else
                    mutexFurnances->unlock();
            }

            //Odłozenie na blat
            while (!havePlace && !end) {
                mvprintw(numb, 0, "Kucharz %d: Odklada pizze na blat          \0", numb);
                usleep(breaks);
                while (!mutexCountertop->try_lock() && !end);

                for (int i = 0; i < counterSize; i++) {
                    if (countertop[i] == nullptr) {
                        havePlace = true;
                        countertop[i] = new Pizza(client);

                        for (int i = 0; i < 20; i++)
                            mvprintw(21, 50 + 3 * i, "   ");
                        mvprintw(20, 50, "Blat\0");
                        for (int i = 0; i < counterSize; i++) {
                            if (countertop[i] != nullptr)
                                mvprintw(21, 50 + 3 * i, "%d", countertop[i]->getClient());
                        }

                        mutexCountertop->unlock();
                        break;
                    }
                }
                if (havePlace)
                    break;
                else
                    mutexCountertop->unlock();
            }

        } else {
            mutexOrdersList->unlock();
            mvprintw(numb, 0, "Kucharz %d: Czeka na zamowienia          \0", numb);
            usleep(breaks);
        }
    }
}

void Cook::threadClose() {
    end = true;
}