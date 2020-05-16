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
         can_exec->first <= order.price;
         ++can_exec) {
        auto& asks = can_exec->second;
        while (order.quantity > 0) {
            auto match = asks.begin();
            if (match->quantity > order.quantity) {
                fmt::print("{} shares were sold at {} USD\n", order.quantity,
                           match->price);
                match->quantity -= order.quantity;
                return;
            } else if (match->quantity < order.quantity) {
                fmt::print("{} shares were sold at {} USD\n", order.quantity,
                           match->price);
                fmt::print("Order partially executed ({} out of {})\n",
                           match->quantity, order.quantity);
                order.quantity -= match->quantity;
                match = asks.erase(match);
                if (asks.empty()) {
                    order_book.asks.erase(can_exec);
                }
            } else {
                fmt::print("{} shares were sold at {} USD\n", order.quantity,
                           match->price);
                match = asks.erase(match);
                if (asks.empty()) {
                    order_book.asks.erase(can_exec);
                }
                return;
            }
        }
    }
    order_book.bids[order.price].push_back(order);
}

void ask(OrderBook& order_book, Order& order) {
    for (auto can_exec = std::begin(order_book.bids);
         can_exec != std::end(order_book.bids) &&
         can_exec->first >= order.price;
         ++can_exec) {
        auto& bids = can_exec->second;
        while (order.quantity > 0) {
            auto match = bids.begin();
            if (match->quantity > order.quantity) {
                fmt::print("{} shares were sold at {} USD\n", order.quantity,
                           match->price);
                match->quantity -= order.quantity;
                return;
            } else if (match->quantity < order.quantity) {
                fmt::print("{} shares were sold at {} USD\n", order.quantity,
                           match->price);
                fmt::print("Order partially executed ({} out of {})\n",
                           match->quantity, order.quantity);
                order.quantity -= match->quantity;
                match = bids.erase(match);
                if (bids.empty()) {
                    order_book.bids.erase(can_exec);
                }
            } else {
                fmt::print("{} shares were sold at {} USD\n", order.quantity,
                           match->price);
                match = bids.erase(match);
                if (bids.empty()) {
                    order_book.bids.erase(can_exec);
                }
                return;
            }
        }
    }
    order_book.asks[order.price].push_back(order);
}