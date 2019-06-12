#ifndef CHAIR_H
#define CHAIR_H

class Client;

class Chair {
public:
	void setClient(Client *c);

	void noClient();

	int getClientType();

	int getClientNumber();

	bool isFree();

private:
	Client *client = nullptr;
};

#endif /* CHAIR_H */