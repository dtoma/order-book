#pragma once

#include <map>

struct Price {
    int value;
};

struct Quantity {
    int value;
};

enum class OrderSide { BUY, SELL };

struct Order {
    int id;
    OrderSide side;
    Quantity quantity;
    Price price;
};

std::map<int, int> bids, asks;