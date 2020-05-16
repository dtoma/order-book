#pragma once

#include <list>
#include <map>

using Price = int;
using Quantity = int;
enum class OrderSide { BUY, SELL };

struct Order {
    int id;
    OrderSide side;
    Quantity quantity;
    Price price;
};

inline bool operator==(Order const& left, Order const& right) {
    return left.id == right.id && left.side == right.side &&
           left.quantity == right.quantity && left.price == right.price;
}

using Book = std::map<Price, std::list<Order>>;

struct OrderBook {
    Book bids;
    Book asks;
};

void display(Book const&);
void cleanup(Book&);
void bid(OrderBook&, Order&);
void ask(OrderBook&, Order&);