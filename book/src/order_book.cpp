#include "order_book.h"

#include <algorithm>

#include "fmt/printf.h"

using namespace ob;

void OrderBook::show_bids() const {
    if (this->bids.empty()) {
        fmt::print("No bids\n");
        return;
    }

    fmt::print("Bids:\n");
    for (auto const &p : this->bids) {
        fmt::print("{}: ", p.first);
        for (auto const &order : p.second) {
            fmt::print("{} ", order.quantity);
        }
        fmt::print("\n");
    }
}

void OrderBook::show_asks() const {
    if (this->asks.empty()) {
        fmt::print("No asks\n");
        return;
    }

    fmt::print("Asks:\n");
    for (auto const &p : this->asks) {
        fmt::print("{}: ", p.first);
        for (auto const &order : p.second) {
            fmt::print("{} ", order.quantity);
        }
        fmt::print("\n");
    }
}

auto OrderBook::execute_at_limit(std::list<Order> &limit_orders, Order &order) {
    for (auto match = std::begin(limit_orders);
         match != std::end(limit_orders) && order.quantity > 0;) {
        if (match->quantity > order.quantity) {
            fmt::print("{} shares were sold at {} USD\n", order.quantity,
                       match->price);
            match->quantity -= order.quantity;
            order.quantity = 0;
            return;
        } else if (match->quantity < order.quantity) {
            fmt::print("{} shares were sold at {} USD\n", match->quantity,
                       match->price);
            order.quantity -= match->quantity;
            match = limit_orders.erase(match);
        } else {
            fmt::print("{} shares were sold at {} USD\n", order.quantity,
                       match->price);
            order.quantity -= match->quantity;
            match = limit_orders.erase(match);
            return;
        }
    }
}

void OrderBook::bid(Order &order) {
    for (auto can_exec = std::begin(this->asks);
         can_exec != std::end(this->asks) && can_exec->first <= order.price;) {
        this->execute_at_limit(can_exec->second, order);
        if (can_exec->second.empty()) {
            can_exec = this->asks.erase(can_exec);
        } else {
            ++can_exec;
        }
    }
    if (order.quantity > 0) {
        this->bids[order.price].push_back(order);
    }
}

void OrderBook::ask(Order &order) {
    for (auto can_exec = std::begin(this->bids);
         can_exec != std::end(this->bids) && can_exec->first >= order.price;) {
        execute_at_limit(can_exec->second, order);
        if (can_exec->second.empty()) {
            can_exec = this->bids.erase(can_exec);
        } else {
            ++can_exec;
        }
    }
    if (order.quantity > 0) {
        this->asks[order.price].push_back(order);
    }
}