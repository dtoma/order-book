#pragma once

namespace obt {

TEST_F(OrderBookTest, TestCancelAsk) {
    ob::Order ask1{1, ob::OrderSide::SELL, 5, 110};
    order_book.ask(ask1);
    ob::Order ask2{2, ob::OrderSide::SELL, 10, 110};
    order_book.ask(ask2);

    auto expected_asks = ob::Asks{{110, {ask1, ask2}}};
    ASSERT_EQ(order_book.asks, expected_asks);

    order_book.cancel(ask1);

    auto expected_remaining_asks = ob::Asks{{110, {ask2}}};
    ASSERT_EQ(order_book.asks, expected_remaining_asks);
}

TEST_F(OrderBookTest, TestCancelBid) {
    ob::Order bid1{1, ob::OrderSide::BUY, 5, 110};
    order_book.bid(bid1);
    ob::Order bid2{2, ob::OrderSide::BUY, 10, 110};
    order_book.bid(bid2);

    auto expected_bids = ob::Bids{{110, {bid1, bid2}}};
    ASSERT_EQ(order_book.bids, expected_bids);

    order_book.cancel(bid1);

    auto expected_remaining_bids = ob::Bids{{110, {bid2}}};
    ASSERT_EQ(order_book.bids, expected_remaining_bids);
}

TEST_F(OrderBookTest, TestCancelAskEmpty) {
    ob::Order ask{1, ob::OrderSide::SELL, 5, 110};
    ASSERT_TRUE(order_book.asks.empty());
    order_book.cancel(ask);
    ASSERT_TRUE(order_book.asks.empty());
}

TEST_F(OrderBookTest, TestCancelBidEmpty) {
    ob::Order bid{1, ob::OrderSide::BUY, 5, 110};
    ASSERT_TRUE(order_book.bids.empty());
    order_book.cancel(bid);
    ASSERT_TRUE(order_book.bids.empty());
}
}  // namespace obt