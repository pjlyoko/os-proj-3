#include "Order.h"
#include "Pizza.h"
#include "Tool.h"
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

#ifndef PIZZAIOLO_H
#define PIZZAIOLO_H

class Pizzaiolo {
public:
	Pizzaiolo(int numb, mutex *mutexOrdersList, vector<Order *> *ordersList, condition_variable *cvClientMadeAnOrder,
			  mutex *mutexFridge, int *fridge,
			  mutex *mutexTools, vector<Tool *> *tools, mutex *mutexFurnaces, int *furnace, mutex *mutexCountertop,
			  Pizza **countertop, mutex *mutexWriter, condition_variable *cv);

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
	int *furnaces;
	Pizza **countertop;
	vector<Tool *> *tools;
	vector<Order *> *ordersList;
	condition_variable *cv, *cvClientMadeAnOrder;

//	int toolTaken = -1;
	Tool *toolTaken = nullptr;
	int furnaceUsed = -1;


	void printFurnacesStatus(int furnacesSize);


	void takeIngredients(int fridgeSize);

	void takeTool();

	void preparePizza(float preparingTime);

	void returnTool();

	void bakePizza(int pizzaSize, float bakeTime, int furnacesSize);

	void takePizzaFromFurnace(int pizzaSize, int furnacesSize);

	void putPizzaOnCountertop(int client, int countertopSize);
};

#endif /* PIZZAIOLO_H */

