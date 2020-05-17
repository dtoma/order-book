#include "order_book.h"
#include "spdlog/spdlog.h"

int main(int ac, char** av) {
    if (ac < 2) {
        spdlog::error("Usage: {} orders_file_path", av[0]);
        return 1;
    }

    std::string file_path(av[1]);
    spdlog::info("Opening file {}", file_path);
    std::ifstream file_stream(file_path);
    spdlog::info("Load orders from file", file_path);
    auto orders = ob::read_orders_file(file_stream);
    spdlog::info("Read {} orders", orders.size());

    ob::OrderBook order_book;
    for (auto& order : orders) {
        order_book.show_bids();
        order_book.show_asks();
        if (order.first == ob::OrderType::NEW) {
            if (order.second.side == ob::OrderSide::BUY) {
                order_book.bid(order.second);
            } else if (order.second.side == ob::OrderSide::SELL) {
                order_book.ask(order.second);
            }
        } else if (order.first == ob::OrderType::CANCEL) {
            order_book.cancel(order.second);
        }
    }

    return 0;
}