#include "order_book.hpp"
#include "order.hpp"
#include <iostream>

// Creating the addBid funtion --> But i have a question why did we use int64_t price in the argument as it could have been easily gotten from order.price ??
void OrderBook::addBid(int64_t price, const Order& order)
{
    auto it = bids_[price];
    it.price = price;
    it.orders = {order};
}

// Creating the addBid funtion --> But i have a question why did we use int64_t price in the argument as it could have been easily gotten from order.price ??
void OrderBook::addAsk(int64_t price, const Order& order)
{
    auto it = ask_[price];
    it.price = price;
    it.orders = {order};
}


int64_t OrderBook::bestAsk() const
{
    if(bids_.empty())
    {
        std::cerr<<"The OrderBook is Empty!"<<'\n'; // I am returning an error and also returning -1 as a sign of error;
        return -1;
    }
    auto it =  bids_.rbegin();
    return it->first;
}

int64_t OrderBook::bestBid() const
{
    if(ask_.empty())
    {
        std::cerr<<"The OrderBook is Empty!"<<'\n';
        return -1;
    }

    auto it =  bids_.begin();
    return it->first;
}