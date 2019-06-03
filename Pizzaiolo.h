#include "Order.h"
#include "Pizza.h"
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <ncurses.h>
#include <ctime>
#include <unistd.h>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <condition_variable>

using namespace std;

#ifndef COOK_H
#define COOK_H

class Pizzaiolo {
public:
	Pizzaiolo(int numb, mutex *mutexOrdersList, vector<Order *> *ordersList, mutex *mutexFridge, int *fridge,
			  mutex *mutexTools, bool *tools, mutex *mutexFurnaces, int *furnace, mutex *mutexCountertop,
			  Pizza **countertop, mutex *mutexWriter);

	void threadStart();

	void threadClose();

	~Pizzaiolo();

private:
	bool end = false;
	int breaks = 500000;
	int numb;
	vector<int> ingredients;
	thread *threadC;

	mutex *mutexOrdersList, *mutexFridge, *mutexTools, *mutexFurnaces, *mutexCountertop, *mutexWriter;
	int *fridge;
	bool *tools;
	int *furnaces;
	Pizza **countertop;
	vector<Order *> *ordersList;

	int toolTaken = -1;
	int furnaceUsed = -1;

	void takeIngredients(int fridgeSize);
	void takeTool(int toolsSize);
	void preparePizza(float preparingTime);
	void returnTool(int toolsSize);
	void bakePizza(int pizzaSize, float bakeTime, int furnacesSize);
	void takePizzaFromFurnace(int pizzaSize, int furnacesSize);
	void putPizzaOnCountertop(int client, int countertopSize);
};

#endif /* COOK_H */

