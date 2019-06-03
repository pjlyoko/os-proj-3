#include "Pizza.h"

Pizza::Pizza(int c) : client(c) {}

Pizza::~Pizza() = default;

int Pizza::getClient() {
	return client;
}