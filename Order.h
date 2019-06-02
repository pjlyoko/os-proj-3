#include <vector>

using namespace std;

#ifndef ORDER_H
#define ORDER_H

class Order {
public:
    Order();
    Order(int c, int s, vector<int>ing);
    Order(const Order& orig);
    virtual ~Order();
    
    vector <int> getIngredients();
    int getClient();
    int getSize();
    
private:
    vector<int> ingredients;
    int client;
    int size;

};

#endif /* ORDER_H */

