
#include "Supplier.h"

Supplier::Supplier() 
{
    end = false;
    breaks = 500000;
}

Supplier::Supplier(const Supplier& orig) {
}

Supplier::Supplier(int numb, mutex* mutexFridge, int* fridge) {
    this->numb = numb;
    this->mutexFridge = mutexFridge;
    this->fridge = fridge;

    end = false;
    breaks = 500000;
    threadS = new thread(&Supplier::threadSuplier, this);
}

Supplier::~Supplier() {
    threadS->join();
    delete threadS;
}

void Supplier::threadSuplier() {
    float deliverTime = 15;
    chrono::_V2::steady_clock::time_point begin, dur;

    int fridgeSize = 10;
    
    while (!end) {
        
        while (!mutexFridge->try_lock() && !end) {
            mvprintw(numb + 30, 0, "Dostawca %d: Czeka na dostęp do lodowki\0", numb);
        }
        mvprintw(numb + 30, 0, "Dostawca %d: uzupelnia lodowke              \0", numb);
        for (int i = 0; i< fridgeSize; i++) {
            fridge[i] = 10;
        }
        usleep(breaks);
        mvprintw(5, 50, "Lodowka");
        for (int i = 0; i < fridgeSize; i++) {
            mvprintw(6, 50 + 3 * i, "%d ", fridge[i]);
        }
        mutexFridge->unlock();

        begin = chrono::steady_clock::now();
        dur = chrono::steady_clock::now();

        while (chrono::duration_cast<chrono::duration<double> >(dur - begin).count() < deliverTime && !end) {
            mvprintw(numb + 30, 0, "Dostawca %d: Wyjechal po skladniki %d %%              \0", numb, (int) round(chrono::duration_cast<chrono::duration<double> >(dur - begin).count() / deliverTime * 100));

            usleep(breaks);
            dur = chrono::steady_clock::now();
        }
    }
}

void Supplier::threadClose() {
    end = true;
}
