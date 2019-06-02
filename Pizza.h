#ifndef PIZZA_H
#define PIZZA_H

class Pizza {
public:
    Pizza();

    Pizza(int c);

    Pizza(const Pizza &orig);

    virtual ~Pizza();

    int getClient();

private:
    int client;

};

#endif /* PIZZA_H */

