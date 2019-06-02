#include "Pizza.h"

Pizza::Pizza() = default;

Pizza::Pizza(const Pizza &orig) {
}

Pizza::Pizza(int c) {
    client = c;
}

Pizza::~Pizza() = default;

int Pizza::getClient() {
    return client;
}