#pragma once
#include <list>
#include "order.hpp"
#include <cstdint>

struct PriceLevel
{
    int64_t price;
    std::list<Order> orders;
};