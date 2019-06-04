#include <utility>

#include "Waiter.h"

Waiter::Waiter(int numb, mutex *mutexCountertop, Pizza **countertop, vector<Client *> clients, mutex *mutexWriter,
               condition_variable *cv) :
        numb(numb), mutexCountertop(mutexCountertop), countertop(countertop), mutexWriter(mutexWriter), cv(cv) {
    this->clients = std::move(clients);
    threadW = new thread(&Waiter::threadWaiter, this);
}

Waiter::~Waiter() {
    threadW->join();
    delete threadW;
}

bool Waiter::pizzasAvailable(int counterSize) {
    for (int i = 0; i < counterSize; i++) {
        if (countertop[i] != nullptr) {
            return true;
        }
    }

    return false;
}

void Waiter::takePizza(int counterSize) {
    bool gotPizza = false;

    while (!gotPizza && !end) {
        {
            unique_lock<mutex> lk_write(*mutexWriter);
            mvprintw(numb + 25, 0, "Kelner %d: Czeka na dostep do blatu    ", numb);
        }

        {
            unique_lock<mutex> lk(*mutexCountertop);

            {
                unique_lock<mutex> lk_write(*mutexWriter);
                mvprintw(numb + 25, 0, "Kelner %d: Czeka na pizze do zaniesienia    ", numb);
            }

            // TODO: Naruszenie ochrony pamięci, gdy aplikacja kończy pracę w trakcie oczekiwania.
            cv->wait(lk, [&] { return pizzasAvailable(counterSize) || end; });

            if(end) {
                break;
            }

            for (int i = 0; i < counterSize; i++) {
                if (countertop[i] != nullptr) {
                    pizza = countertop[i];
                    gotPizza = true;
                    countertop[i] = nullptr;

                    {
                        unique_lock<mutex> lk_write(*mutexWriter);

                        for (int j = 0; j < 20; j++) {
                            mvprintw(21, 50 + 3 * j, "   ");
                        }
                        mvprintw(20, 50, "Blat");
                        for (int j = 0; j < counterSize; j++) {
                            if (countertop[j] != nullptr) {
                                mvprintw(21, 50 + 3 * j, "%d", countertop[j]->getClient());
                            }
                        }
                    }

                    break;
                }
            }

        }

        if (gotPizza) {
            break;
        }

        usleep(breaks);
    }
}

void Waiter::deliverPizza() {
    float deliverTime = (random() % 10) / 5. + 1;
    auto begin = chrono::steady_clock::now();
    auto dur = chrono::steady_clock::now();

    while (chrono::duration_cast<chrono::duration<double> >(dur - begin).count() < deliverTime && !end) {
        {
            unique_lock<mutex> lk_write(*mutexWriter);
            mvprintw(numb + 25, 0, "Kelner %d: Zanosi pizze klientowi %2d   %d%%        ", numb, pizza->getClient(),
                     (int) round(chrono::duration_cast<chrono::duration<double> >(dur - begin).count() / deliverTime *
                                 100));
        }

        usleep(breaks);
        dur = chrono::steady_clock::now();
    }

    clients[pizza->getClient()]->setPizza(pizza);
    pizza = nullptr;
}


void Waiter::threadWaiter() {
    int counterSize = 10;

    while (!end) {
        // Podnosi pizzę z blatu
        takePizza(counterSize);

        // Zanosi pizzę klientowi
        deliverPizza();
    }
}

void Waiter::threadClose() {
    end = true;
}