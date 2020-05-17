#include "order_book.h"

#include <algorithm>

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

//! Local helper functions
namespace {

/** \brief Tokenize an iterable using a given separator & callable.
 *
 * From: http://tristanbrindle.com/posts/a-quicker-study-on-tokenising/
 */
template <typename Iterator, typename Lambda>
void for_each_token(Iterator &&first, Iterator &&last, char separator,
                    Lambda &&lambda) {
    while (first != last) {
        const auto pos = std::find(first, last, separator);

        lambda(first, pos);

        if (pos == last) {
            break;
        }

        first = std::next(pos);
    }
}

//! Build an Order object from string values.
ob::Order order_from_strings(std::string id, std::string side,
                             std::string quantity, std::string price) {
    return {std::stoi(id),
            side == "B" ? ob::OrderSide::BUY : ob::OrderSide::SELL,
            std::stoi(quantity), std::stoi(price)};
}

//! Print a table (templated to support maps with different orderings)
template <typename T>
void show_table(T const &table) {
    for (auto const &p : table) {
        spdlog::info("  {}: ", p.first);
        for (auto const &order : p.second) {
            spdlog::info("    {} #{}", order.quantity, order.id);
        }
    }
}

}  // namespace

/**
 * Library implementation
 */
namespace ob {

void OrderBook::show_bids() const {
    if (this->bids.empty()) {
        spdlog::info("No bids");
        return;
    }

    spdlog::info("===== Bids =====");
    show_table(this->bids);
}

void OrderBook::show_asks() const {
    if (this->asks.empty()) {
        spdlog::info("No asks");
        return;
    }

    spdlog::info("===== Asks =====");
    show_table(this->asks);
}

auto OrderBook::execute_at_limit(std::vector<Order> &limit_orders,
                                 Order &order) {
    auto potential_match = std::begin(limit_orders);
    while (potential_match != std::end(limit_orders) && order.quantity > 0) {
        if (potential_match->quantity > order.quantity) {
            // Full execution, leftover quantity in the book
            spdlog::info("{} shares sold at {}", order.quantity,
                         potential_match->price);
            potential_match->quantity -= order.quantity;
            order.quantity = 0;
            return;
        } else if (potential_match->quantity < order.quantity) {
            // Partial execution
            spdlog::info("{} shares sold at {}", potential_match->quantity,
                         potential_match->price);
            order.quantity -= potential_match->quantity;
            potential_match = limit_orders.erase(potential_match);
        } else {
            // Full execution without leftover quantity in the book
            spdlog::info("{} shares sold at {}", order.quantity,
                         potential_match->price);
            order.quantity -= potential_match->quantity;
            potential_match = limit_orders.erase(potential_match);
            return;
        }
    }
}

void OrderBook::bid(Order &order) {
    auto limit = std::begin(this->asks);
    auto stop = std::end(this->asks);
    while (limit != stop && limit->first <= order.price) {
        this->execute_at_limit(limit->second, order);
        if (limit->second.empty()) {
            limit = this->asks.erase(limit);
        } else {
            ++limit;
        }
    }
    if (order.quantity > 0) {
        spdlog::info("Add a new bid id={} quantity={} price={}", order.id,
                     order.quantity, order.price);
        this->bids[order.price].push_back(order);
    }
}

void OrderBook::ask(Order &order) {
    auto limit = std::begin(this->bids);
    while (limit != std::end(this->bids) && limit->first >= order.price) {
        execute_at_limit(limit->second, order);
        if (limit->second.empty()) {
            limit = this->bids.erase(limit);
        } else {
            ++limit;
        }
    }
    if (order.quantity > 0) {
        spdlog::info("Add a new ask id={} quantity={} price={}", order.id,
                     order.quantity, order.price);
        this->asks[order.price].push_back(order);
    }
}

void OrderBook::place_order(Order &order) {
    if (order.side == ob::OrderSide::BUY) {
        this->bid(order);
    } else if (order.side == ob::OrderSide::SELL) {
        this->ask(order);
    }
}

void OrderBook::cancel(Order const &order) {
    spdlog::info("Trying to cancel id={}", order.id);
    if (order.side == OrderSide::BUY) {
        auto limit = this->bids[order.price];
        auto to_delete = std::find_if(std::begin(limit), std::end(limit),
                                      [&](auto o) { return o.id == order.id; });
        if (to_delete == std::end(limit)) {
            spdlog::info("Order id={} not found", order.id);
            return;
        }
        limit.erase(to_delete);
        spdlog::info("Cancelled order id={}", order.id);
        if (limit.empty()) {
            this->bids.erase(order.price);
        }
    } else if (order.side == OrderSide::SELL) {
        auto limit = this->asks[order.price];
        auto to_delete = std::find_if(std::begin(limit), std::end(limit),
                                      [&](auto o) { return o.id == order.id; });
        if (to_delete == std::end(limit)) {
            spdlog::info("Order id={} not found", order.id);
            return;
        }
        limit.erase(to_delete);
        spdlog::info("Cancelled order id={}", order.id);
        if (limit.empty()) {
            this->asks.erase(order.price);
        }
    }
}

std::vector<std::pair<OrderType, Order>> read_orders_file(
    std::ifstream &file_stream) {
    std::vector<std::pair<OrderType, Order>> orders;
    std::vector<std::string> tokens;
    tokens.reserve(5);

    for (std::string line; std::getline(file_stream, line);) {
        spdlog::info(line);
        for_each_token(std::begin(line), std::end(line), ',',
                       [&tokens](auto start, auto finish) {
                           tokens.emplace_back(start, finish);
                       });
        auto order_type = StringToOrderType.find(tokens[0]);
        if (order_type == std::end(StringToOrderType)) {
            spdlog::error("Invalid order type in line [{}]", line);
            continue;
        }
        orders.emplace_back(
            order_type->second,
            order_from_strings(tokens[1], tokens[2], tokens[3], tokens[4]));
        tokens.clear();
    }
    return orders;
}

}  // namespace ob