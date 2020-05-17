#pragma once

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "spdlog/fmt/ostr.h"

namespace ob {

using Price = int;
using Quantity = int;
enum class OrderSide { BUY, SELL };
enum class OrderType { NEW, CANCEL };

static const std::map<std::string, OrderType> StringToOrderType{
    {"A", OrderType::NEW}, {"X", OrderType::CANCEL}};

struct Order final {
    int id;
    OrderSide side;
    Quantity quantity;
    Price price;
};

inline bool operator==(Order const &left, Order const &right) {
    return left.id == right.id && left.side == right.side &&
           left.quantity == right.quantity && left.price == right.price;
}

/**
 * Using std:map is convenient because it is ordered and we can easily customize
 * the ordering.
 */
using Asks = std::map<Price, std::vector<Order>>;
using Bids = std::map<Price, std::vector<Order>, std::greater<Price>>;

struct OrderBook final {
    Bids bids;
    Asks asks;

    void show_bids() const;
    void show_asks() const;

    auto execute_at_limit(std::vector<Order> &, Order &);
    void bid(Order &);
    void ask(Order &);
    void cancel(Order const &);
    void place_order(Order &);
};

std::vector<std::pair<OrderType, Order>> read_orders_file(std::ifstream &);

}  // namespace ob