#include "order_book.h"

#include <algorithm>

#include "fmt/printf.h"

void display(Book const& book) {
    if (book.empty()) {
        fmt::print("Empty book\n");
        return;
    }

    for (auto const& p : book) {
        fmt::print("{}: ", p.first);
        for (auto const& order : p.second) {
            fmt::print("{} ", order.quantity);
        }
        fmt::print("\n");
    }
}

/**
 * We can't use remove_if on std::map so we need a custom function.
 * In C++20 we'll have map::erase_if.
 */
void cleanup(Book& book) {
    auto end = std::end(book);
    for (auto it = std::begin(book); it != end; ++it) {
        if (it->second.empty()) {
            book.erase(it);
        }
    }
}

auto execute_at_limit(std::list<Order>& limit_orders, Order& order) {
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

/** Bid & Ask
 * 1. Try to execute the order as much as possible
 * 2. Insert the remaining quantity into the appropriate order book
 */

void bid(OrderBook& order_book, Order& order) {
    /**
     * While we have orders at or above `order.price`,
     * execute.
     */
    for (auto can_exec = std::begin(order_book.asks);
         can_exec != std::end(order_book.asks) &&
         can_exec->first <= order.price;) {
        execute_at_limit(can_exec->second, order);
        if (can_exec->second.empty()) {
            can_exec = order_book.asks.erase(can_exec);
        } else {
            ++can_exec;
        }
    }
    if (order.quantity > 0) {
        order_book.bids[order.price].push_back(order);
    }
}

void ask(OrderBook& order_book, Order& order) {
    for (auto can_exec = std::begin(order_book.bids);
         can_exec != std::end(order_book.bids) &&
         can_exec->first >= order.price;) {
        execute_at_limit(can_exec->second, order);
        if (can_exec->second.empty()) {
            can_exec = order_book.bids.erase(can_exec);
        } else {
            ++can_exec;
        }
    }
    if (order.quantity > 0) {
        order_book.asks[order.price].push_back(order);
    }
}