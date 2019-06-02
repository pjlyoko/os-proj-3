#include "Order.h"

Order::Order() = default;

Order::Order(int c, int s, std::vector<int> ing) {

    for (int i : ing) {
        ingredients.push_back(i);
    }

    client = c;
    size = s;
}

Order::Order(const Order &orig) {
}

Order::~Order() = default;

std::vector<int> Order::getIngredients() {
    return ingredients;
}

int Order::getClient() {
    return client;
}

int Order::getSize() {
    return size;
}