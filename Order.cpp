
#include "Order.h"

Order::Order() {
}

Order::Order(int c, int s, vector <int> ing) {

    for (int i = 0; i<ing.size(); i++) {
        ingredients.push_back(ing[i]);
    }

    client = c;
    size = s;
}

Order::Order(const Order& orig) {
}

Order::~Order() {
}

vector <int> Order::getIngredients() {
    return ingredients;
}

int Order::getClient() {
    return client;
}

int Order::getSize() {
    return size;
}