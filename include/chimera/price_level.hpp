#pragma once
#include <list>
#include "order.hpp"

struct PriceLevel
{
    int price;
    std::list<Order> orders;
};