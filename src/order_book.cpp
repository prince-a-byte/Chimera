#include "order_book.hpp"
#include "order.hpp"
#include <iostream>

// Creating the addBid funtion --> But i have a question why did we use int64_t price in the argument as it could have been easily gotten from order.price ??
void OrderBook::addBid(int64_t price, const Order& order)
{
    auto& it = bids_[price];
    it.price = price;
    it.orders.push_back(order);
}

// Creating the addBid funtion --> But i have a question why did we use int64_t price in the argument as it could have been easily gotten from order.price ??
void OrderBook::addAsk(int64_t price, const Order& order)
{
    auto& it = ask_[price];
    it.price = price;
    it.orders.push_back(order);
}


int64_t OrderBook::bestAsk() const
{
    if(ask_.empty())
        return -1;

        return ask_.begin()->first;
}

int64_t OrderBook::bestBid() const
{
    if (bids_.empty())
        return -1;

    return bids_.rbegin()->first;
}

// Creating a OrderCount
size_t OrderBook::bidOrderCount(int64_t price) const
{
    auto it = bids_.find(price);

    if(it == bids_.end())
        return 0;

    return it->second.orders.size();
}

size_t OrderBook::askOrderCount(int64_t price) const
{
    auto it = ask_.find(price);

    if(it == ask_.end())
        return 0;

    return it->second.orders.size();
}