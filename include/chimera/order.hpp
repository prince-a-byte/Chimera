#pragma once
#include <cstdint>

enum class Side {BUY, SELL};

struct Order
{
    uint64_t id;
    int64_t price;
    uint64_t originalQuantity;
    uint64_t filledQuantity = 0;
    Side side;
    uint64_t timeStamp;
};

uint64_t remainingQuantity(const Order& order);

bool isFullyFilled(const Order& order);

void applyFill(Order& order, uint64_t fillQuantity);