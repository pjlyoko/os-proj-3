#ifndef PIZZA_H
#define PIZZA_H

class Pizza {
public:
	explicit Pizza(int c);

    ~Pizza();

    int getClient();

private:
    int client;

};

#endif /* PIZZA_H */

