#ifndef SO2_CHAIR_H
#define SO2_CHAIR_H

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


#endif //SO2_CHAIR_H
