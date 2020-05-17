#include "order_book.h"

#include <algorithm>

#include "fmt/printf.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

/**
 * Helper functions
 */
namespace {

// http://tristanbrindle.com/posts/a-quicker-study-on-tokenising/
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

ob::Order order_from_strings(std::string id, std::string side,
                             std::string quantity, std::string price) {
    return {std::stoi(id),
            side == "B" ? ob::OrderSide::BUY : ob::OrderSide::SELL,
            std::stoi(quantity), std::stoi(price)};
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

    spdlog::info("Bids:");
    for (auto const &p : this->bids) {
        spdlog::info("  {}: ", p.first);
        for (auto const &order : p.second) {
            spdlog::info("    #{} {}", order.id, order.quantity);
        }
    }
}

void OrderBook::show_asks() const {
    if (this->asks.empty()) {
        spdlog::info("No asks");
        return;
    }

    spdlog::info("Asks:");
    for (auto const &p : this->asks) {
        spdlog::info("  {}: ", p.first);
        for (auto const &order : p.second) {
            spdlog::info("    #{} {}", order.id, order.quantity);
        }
    }
}

auto OrderBook::execute_at_limit(std::vector<Order> &limit_orders,
                                 Order &order) {
    for (auto match = std::begin(limit_orders);
         match != std::end(limit_orders) && order.quantity > 0;) {
        if (match->quantity > order.quantity) {
            spdlog::info("{} shares were sold at {} USD", order.quantity,
                         match->price);
            match->quantity -= order.quantity;
            order.quantity = 0;
            return;
        } else if (match->quantity < order.quantity) {
            spdlog::info("{} shares were sold at {} USD", match->quantity,
                         match->price);
            order.quantity -= match->quantity;
            match = limit_orders.erase(match);
        } else {
            spdlog::info("{} shares were sold at {} USD", order.quantity,
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
        spdlog::info("Add a new bid id={} quantity={} price={}", order.id,
                     order.quantity, order.price);
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
        spdlog::info("Add a new ask id={} quantity={} price={}", order.id,
                     order.quantity, order.price);
        this->asks[order.price].push_back(order);
    }
}

void OrderBook::cancel(Order const &order) {
    if (order.side == OrderSide::BUY) {
        auto limit = this->bids[order.price];
        auto to_delete = std::find_if(std::begin(limit), std::end(limit),
                                      [&](auto o) { return o.id == order.id; });
        if (to_delete == std::end(limit)) {
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
            spdlog::error("Invalid line [{}]", line);
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