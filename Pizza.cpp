#include "Pizza.h"

Pizza::Pizza(int c) : client(c) {}

int Pizza::getClient() {
	return client;
}