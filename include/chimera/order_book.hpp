#pragma once
#include <map>
#include "price_level.hpp"

class OrderBook
{
    public:
    void addBid(int64_t price, const Order& order);
    void addAsk(int64_t price, const Order& order);
    
    int64_t bestBid() const;
    int64_t bestAsk() const;

    size_t bidOrderCount(int64_t price) const;
    size_t askOrderCount(int64_t price) const;

    private:
    std::map<int64_t, PriceLevel> bids_;
    std::map<int64_t, PriceLevel> ask_;

};