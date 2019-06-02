#include <vector>

#ifndef ORDER_H
#define ORDER_H

class Order {
public:
    Order();

    Order(int c, int s, const std::vector<int>& ing);

    Order(const Order &orig);

    virtual ~Order();

    std::vector<int> getIngredients();

    int getClient();

    int getSize();

private:
    std::vector<int> ingredients;
    int client;
    int size;

};

#endif /* ORDER_H */

