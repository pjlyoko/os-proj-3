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
