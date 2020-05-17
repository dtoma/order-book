#include "order_book.h"
#include "spdlog/spdlog.h"

int main(int ac, char** av) {
    if (ac != 2) {
        spdlog::error("Usage: {} orders_file_path", av[0]);
        return 1;
    }

    std::string file_path(av[1]);

    spdlog::debug("Opening file {}", file_path);
    std::ifstream file_stream(file_path);
    if (!file_stream) {
        spdlog::error("Could not open file {}", file_path);
        return 1;
    }

    spdlog::debug("Load orders from file", file_path);
    auto orders = ob::read_orders_file(file_stream);
    spdlog::debug("Loaded {} orders", orders.size());

    ob::OrderBook order_book;

    for (auto& type_and_order : orders) {
        auto& order = type_and_order.second;

        switch (type_and_order.first) {
            case ob::OrderType::NEW:
                order_book.place_order(order);
                break;
            case ob::OrderType::CANCEL:
                order_book.cancel(order);
                break;
        }

        spdlog::debug("Order book is now:");
        order_book.show_bids();
        order_book.show_asks();
    }

    spdlog::info("Final order book:");
    order_book.show_bids(spdlog::level::info);
    order_book.show_asks(spdlog::level::info);

    return 0;
}