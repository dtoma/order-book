#include <benchmark/benchmark.h>

#include "order_book.h"
#include "spdlog/spdlog.h"

static void BM_Bid(benchmark::State& state) {
    spdlog::set_level(spdlog::level::critical);
    ob::OrderBook order_book;
    ob::Order order_buy{1, ob::OrderSide::BUY, 10, 5};

    for (auto _ : state) {
        order_book.bid(order_buy);
    }
}
BENCHMARK(BM_Bid);

static void BM_Ask(benchmark::State& state) {
    spdlog::set_level(spdlog::level::critical);
    ob::OrderBook order_book;
    ob::Order order_sell{1, ob::OrderSide::SELL, 10, 5};

    for (auto _ : state) {
        order_book.ask(order_sell);
    }
}
BENCHMARK(BM_Ask);

static void BM_Ask_Exec(benchmark::State& state) {
    spdlog::set_level(spdlog::level::critical);

    ob::OrderBook order_book;

    ob::Order order_buy1{1, ob::OrderSide::BUY, 10000, 5};
    order_book.bid(order_buy1);

    ob::Order order_buy2{1, ob::OrderSide::BUY, 10000, 6};
    order_book.bid(order_buy2);

    ob::Order order_sell{3, ob::OrderSide::SELL, 1, 5};

    for (auto _ : state) {
        order_book.ask(order_sell);
    }
}
BENCHMARK(BM_Ask_Exec);

BENCHMARK_MAIN();