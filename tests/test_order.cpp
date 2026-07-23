#include "order.hpp"
#include <iostream>
#include <cassert>

int main()
{
    Order order;
    order.id = 1;
    order.price = 150;
    order.originalQuantity=100;
    order.side = Side::BUY;
    order.timeStamp = 102410;


    // ...
    applyFill(order, 75);
    assert(order.filledQuantity == 75);
    assert(remainingQuantity(order) == 25);
    assert(!isFullyFilled(order));


    return 0;
}