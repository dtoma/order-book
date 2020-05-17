#pragma once

#include "fixtures.h"

namespace obt {
TEST_F(OrderBookTest, TestAddOrders) {
    ob::Order order_buy{1, ob::OrderSide::BUY, 10, 5};
    ob::Order order_sell{2, ob::OrderSide::SELL, 10, 10};

    order_book.bid(order_buy);
    ASSERT_EQ(order_book.bids[order_buy.price].front(), order_buy);

    order_book.ask(order_sell);
    ASSERT_EQ(order_book.asks[order_sell.price].front(), order_sell);
}
}