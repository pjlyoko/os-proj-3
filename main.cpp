#include "Cook.h"
#include"Client.h"
#include "Waiter.h"
#include "Supplier.h"
#include "Pizza.h"
#include <vector>
#include <iostream>
#include <thread>
#include <ncurses.h>

using namespace std;

int main() {
    srand(time(NULL));
    initscr();
    char x;

    vector<Cook*> cooks;
    vector<Client*> clients;
    vector<Waiter*> waiters;
    vector<Supplier*> suppliers;

    vector<Order*> *ordersList = new vector<Order*>();
    mutex mutexOrdersList, mutexFridge, mutexTools, mutexFurnances, mutexCountertop, mutexChairs;
    int fridge[10] = {0,0,0,0,0,0,0,0,0,0};
    bool tools[4] {
        true, true, true, true
    };
    int furnances[5] {
        2, 2, 2, 2, 2
    };
    Pizza*countertop[10]{
        NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
    };

    bool chairs[10] {
        true, true, true, true, true, true, true, true, true, true
    };
    mvprintw(0, 0, "Dziala");

    //tworzenie wątków
    for (int i = 0; i < 11; i++) {
        clients.push_back(new Client(i, & mutexChairs, chairs, & mutexOrdersList, ordersList));
    }
    
    for (int i = 0; i < 7; i++) {
        cooks.push_back(new Cook(i, &mutexOrdersList, ordersList, &mutexFridge, fridge, &mutexTools, tools, &mutexFurnances, furnances, &mutexCountertop, countertop));
    }
    
    for (int i = 0; i < 1; i++) {
        suppliers.push_back(new Supplier(i, &mutexFridge, fridge));
    }
    
    for (int i = 0; i < 2; i++) {
        waiters.push_back(new Waiter( i, &mutexCountertop, countertop, clients));
    }

    do {
        timeout(3);
        x = getch();

        if(x == 'c')
        {
            clear();
            x =' ';
        }
    } while (x != 'x');

    //Zamykanie wątków i konczenie 
    for (int i = cooks.size() - 1; i > 0; i--) {
        cooks[i]->threadClose();
        cooks.pop_back();
    }


    for (int i = clients.size() - 1; i > 0; i--) {
        clients[i]->threadClose();
        clients.pop_back();
    }
    
    for (int i = waiters.size() - 1; i>0; i--) {
        waiters[i]->threadClose();
        waiters.pop_back();

    }
    for (int i = suppliers.size() - 1; i>0; i--) {
        suppliers[i]->threadClose();
        suppliers.pop_back();
    }

    endwin();
    return 0;
}

