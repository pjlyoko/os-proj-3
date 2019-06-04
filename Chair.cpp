#include "Chair.h"
#include "Client.h"

void Chair::setClient(Client *c) {
	this->client = c;
}

int Chair::getClientType() {
	if(this->client == nullptr) {
		return -1;
	}
	return this->client->getClientType();
}

int Chair::getClientNumber() {
	if(this->client == nullptr) {
		return -1;
	}
	return this->client->getClientNumber();
}

void Chair::noClient() {
	this->client = nullptr;
}

bool Chair::isFree() {
	return this->client == nullptr;
}