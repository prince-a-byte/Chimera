#include "order.hpp"
#include <cassert>
#include <iostream>

void testPartialFill()
{
    Order order;
    order.id = 1;
    order.price = 150;
    order.originalQuantity = 100;
    order.filledQuantity = 0;
    order.side = Side::BUY;
    order.timeStamp = 102410;

    applyFill(order, 75);

    assert(order.filledQuantity == 75);
    assert(remainingQuantity(order) == 25);
    assert(!isFullyFilled(order));
}

void testExactFill()
{
    Order order;
    order.id = 2;
    order.price = 150;
    order.originalQuantity = 100;
    order.filledQuantity = 0;
    order.side = Side::BUY;
    order.timeStamp = 102411;

    applyFill(order, 100);

    assert(order.filledQuantity == 100);
    assert(remainingQuantity(order) == 0);
    assert(isFullyFilled(order));
}

void testRejectedOverfill()
{
    Order order;
    order.id = 3;
    order.price = 150;
    order.originalQuantity = 100;
    order.filledQuantity = 0;
    order.side = Side::BUY;
    order.timeStamp = 102412;

    applyFill(order, 150);   // exceeds remaining — should be a silent no-op

    assert(order.filledQuantity == 0);
    assert(remainingQuantity(order) == 100);
    assert(!isFullyFilled(order));
}

int main()
{
    testPartialFill();
    testExactFill();
    testRejectedOverfill();

    std::cout << "✅ All Order tests passed!\n";

    return 0;
}