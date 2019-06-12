#include "Order.h"

Order::Order(int c, int s, const std::vector<int> &ing) : client(c), size(s) {
	for(int i : ing) {
		ingredients.push_back(i);
	}
}

std::vector<int> Order::getIngredients() {
	return ingredients;
}

int Order::getClient() {
	return client;
}

int Order::getSize() {
	return size;
}