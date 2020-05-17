#pragma once

#include <list>
#include <map>

namespace ob {

using Price = int;
using Quantity = int;
enum class OrderSide { BUY, SELL };

struct Order final {
    int id;
    OrderSide side;
    Quantity quantity;
    Price price;
};

inline bool operator==(Order const& left, Order const& right) {
    return left.id == right.id && left.side == right.side &&
           left.quantity == right.quantity && left.price == right.price;
}

using Asks = std::map<Price, std::list<Order>>;
using Bids = std::map<Price, std::list<Order>, std::greater<Price>>;

struct OrderBook final {
    Bids bids;
    Asks asks;

    void show_bids() const;
    void show_asks() const;

    auto execute_at_limit(std::list<Order>&, Order&);
    void bid(Order&);
    void ask(Order&);
};

}  // namespace ob