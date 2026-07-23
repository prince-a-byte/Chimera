#include "order.hpp"

// Updating the remaining quantity
uint64_t remainingQuantity(const Order& order)
{
    return order.originalQuantity - order.filledQuantity;
}

// Checking if the order is fully filled or not
bool isFullyFilled(const Order& order)
{
    return order.filledQuantity == order.originalQuantity;
}

// Updating the Filled Quantity
void applyFill(Order& order, uint64_t fillQuantity) // We are using Order& order instead of const Order& order beacuse of the need for updating the value inside.
{
    if(order.originalQuantity >= (order.filledQuantity + fillQuantity))
    {
        order.filledQuantity += fillQuantity;
    }
}