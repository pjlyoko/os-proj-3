#ifndef ORDER_H
#define ORDER_H

#include <vector>

class Order {
public:
	Order(int c, int s, const std::vector<int> &ing);

	std::vector<int> getIngredients();

	int getClient();

	int getSize();

private:
	std::vector<int> ingredients;
	int client;
	int size;

};

#endif /* ORDER_H */