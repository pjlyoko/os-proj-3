
#include "Pizza.h"

Pizza::Pizza() {
}

Pizza::Pizza(const Pizza& orig) {
}

Pizza::Pizza(int c) {
    client = c;
}

Pizza::~Pizza() {
}

int Pizza::getClient() {
    return client;
}