#pragma once

#include "fixtures.h"

namespace obt {
TEST_F(OrderBookTest, TestAddOrders) {
    int bid_price = 5;
    ob::Order order_buy{1, ob::OrderSide::BUY, 10, bid_price};

    int ask_price = 10;
    ob::Order order_sell{2, ob::OrderSide::SELL, 10, ask_price};

    order_book.bid(order_buy);
    auto expected_bids = ob::Bids{{bid_price, {order_buy}}};
    ASSERT_EQ(order_book.bids, expected_bids);

    order_book.ask(order_sell);
    auto expected_asks = ob::Asks{{ask_price, {order_sell}}};
    ASSERT_EQ(order_book.asks, expected_asks);
}
}