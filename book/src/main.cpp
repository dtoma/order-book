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
    for (auto& type_and_order : orders) {
        order_book.show_bids();
        order_book.show_asks();
        if (type_and_order.first == ob::OrderType::NEW) {
            order_book.place_order(type_and_order.second);
        } else if (type_and_order.first == ob::OrderType::CANCEL) {
            order_book.cancel(type_and_order.second);
        }
    }

    spdlog::info("Final order book:");
    order_book.show_bids();
    order_book.show_asks();

    return 0;
}