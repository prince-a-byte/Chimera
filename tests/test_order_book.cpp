#include <cassert>
#include <iostream>

#include "order.hpp"
#include "order_book.hpp"

void testEmptyBook()
{
    OrderBook book;

    assert(book.bestBid() == -1);
    assert(book.bestAsk() == -1);
}

void testSingleOrders()
{
    OrderBook book;

    Order buyOrder;
    buyOrder.id = 12345;
    buyOrder.price = 50;
    buyOrder.originalQuantity = 100;
    buyOrder.timeStamp = 109323;
    buyOrder.side = Side::BUY;

    Order sellOrder;
    sellOrder.id = 1234;
    sellOrder.price = 100;
    sellOrder.originalQuantity = 200;
    sellOrder.timeStamp = 109323;
    sellOrder.side = Side::SELL;

    book.addBid(buyOrder.price, buyOrder);
    book.addAsk(sellOrder.price, sellOrder);

    assert(book.bestBid() == 50);
    assert(book.bestAsk() == 100);
}

void testMultiplePriceLevels()
{
    OrderBook book;

    Order buy1;
    buy1.id = 1;
    buy1.price = 50;
    buy1.originalQuantity = 100;
    buy1.timeStamp = 1;
    buy1.side = Side::BUY;

    Order buy2;
    buy2.id = 2;
    buy2.price = 500;
    buy2.originalQuantity = 100;
    buy2.timeStamp = 2;
    buy2.side = Side::BUY;

    Order buy3;
    buy3.id = 3;
    buy3.price = 200;
    buy3.originalQuantity = 100;
    buy3.timeStamp = 3;
    buy3.side = Side::BUY;

    Order sell1;
    sell1.id = 4;
    sell1.price = 100;
    sell1.originalQuantity = 100;
    sell1.timeStamp = 4;
    sell1.side = Side::SELL;

    Order sell2;
    sell2.id = 5;
    sell2.price = 80;
    sell2.originalQuantity = 100;
    sell2.timeStamp = 5;
    sell2.side = Side::SELL;

    Order sell3;
    sell3.id = 6;
    sell3.price = 10;
    sell3.originalQuantity = 100;
    sell3.timeStamp = 6;
    sell3.side = Side::SELL;

    book.addBid(buy1.price, buy1);
    book.addBid(buy2.price, buy2);
    book.addBid(buy3.price, buy3);

    book.addAsk(sell1.price, sell1);
    book.addAsk(sell2.price, sell2);
    book.addAsk(sell3.price, sell3);

    assert(book.bestBid() == 500);
    assert(book.bestAsk() == 10);
}

void testDuplicatePriceLevel()
{
    OrderBook book;

    Order buy1{};
    buy1.id = 1;
    buy1.price = 100;

    Order buy2{};
    buy2.id = 2;
    buy2.price = 100;

    book.addBid(100, buy1);
    book.addBid(100, buy2);

    assert(book.bestBid() == 100);

    // NEW
    assert(book.bidOrderCount(100) == 2);
}


int main()
{
    testEmptyBook();
    testSingleOrders();
    testMultiplePriceLevels();
    testDuplicatePriceLevel();

    return 0;
}