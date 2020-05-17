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
    spdlog::info("Loaded {} orders", orders.size());

    spdlog::info("Initial order book:");
    ob::OrderBook order_book;
    order_book.show_bids();
    order_book.show_asks();

    for (auto& type_and_order : orders) {
        spdlog::info("\n");
        auto& order = type_and_order.second;
        spdlog::info("Order id={} quantity={} price={}", order.id,
                     order.quantity, order.price);

        switch (type_and_order.first) {
            case ob::OrderType::NEW:
                order_book.place_order(order);
                break;
            case ob::OrderType::CANCEL:
                order_book.cancel(order);
                break;
        }

        spdlog::info("Order book is now:");
        order_book.show_bids();
        order_book.show_asks();
    }

    spdlog::info("\n");

    spdlog::info("Final order book:");
    order_book.show_bids();
    order_book.show_asks();

    return 0;
}