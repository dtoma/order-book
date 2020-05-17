#pragma once

#include "fixtures.h"

namespace obt {
TEST_F(OrderBookTest, TestBidExecutionSimpleMatch) {
    ob::Order order_buy{1, ob::OrderSide::BUY, 10, 5};
    ob::Order order_sell{2, ob::OrderSide::SELL, 10, 5};

    order_book.ask(order_sell);
    auto expected_asks = ob::Asks{{5, {order_sell}}};
    ASSERT_EQ(order_book.asks, expected_asks);

    order_book.bid(order_buy);
    ASSERT_TRUE(order_book.bids.empty())
        << "The bid should have been fully executed";
    ASSERT_TRUE(order_book.asks.empty())
        << "The ask should have been fully executed";
}

/**
 * This is one of the examples given in the email, without the BID side as it's
 * not relevant.
 */
TEST_F(OrderBookTest, TestBidExecutionMultipleMatches) {
    /**
     * =================
     * ASK
     * 110: 5 10
     * 105: 3 7
     * ------------
     * BID
     * =================
     */

    ob::Order ask1{1, ob::OrderSide::SELL, 5, 110};
    order_book.ask(ask1);
    ob::Order ask2{2, ob::OrderSide::SELL, 10, 110};
    order_book.ask(ask2);
    ob::Order ask3{3, ob::OrderSide::SELL, 3, 105};
    order_book.ask(ask3);
    ob::Order ask4{4, ob::OrderSide::SELL, 7, 105};
    order_book.ask(ask4);

    auto expected_asks = ob::Asks{
        {110, {ask1, ask2}},  // we should have these 2 orders at the same limit
        {105, {ask3, ask4}}   // and these 2 here
    };
    ASSERT_EQ(order_book.asks, expected_asks);

    /**
     * Now, let’s imagine some buyer places an “aggressive” order to buy 4
     * shares at the price of 105 USD. It will be an order which actually
     * matches the best price from sellers (namely lowest price at 105 USD). The
     * seller’s order of 3 shares at price 105 USD was added earlier than
     * another seller’s order of 7 shares at 105 USD.
     * Therefore seller’s order of 3 shares will be matched first.
     * As a result we will see a trade: `3 shares of XYZ were sold at 105 USD`.
     * But we still have 1 share left from an “aggressive” buyer who wants to
     * buy at 105 USD. Therefore we will have partial match with seller’s order
     * at 105 USD. We will see second trade happening: 1 share of XYZ were sold
     * at 105 USD
     */

    ob::Order bid1{5, ob::OrderSide::BUY, 4, 105};
    order_book.bid(bid1);
    ASSERT_TRUE(order_book.bids.empty())
        << "The bid should have been fully executed";

    /**
     * =================
     * ASK
     * 110: 5 10
     * 105: 6
     * ------------
     * BID
     * =================
     */

    auto expected_remaining_asks = ob::Asks{
        {110, {ask1, ask2}},  // This limit is left unchanged
        {105, {{4, ob::OrderSide::SELL, 6, 105}}}  // Leftover order #4
    };
    ASSERT_EQ(order_book.asks, expected_remaining_asks);
}

/**
 * This is also an example from the email.
 */
TEST_F(OrderBookTest, TestAskExecutionMultipleMatches) {
    ob::Order ask1{1, ob::OrderSide::SELL, 5, 110};
    order_book.ask(ask1);
    ob::Order ask2{2, ob::OrderSide::SELL, 10, 110};
    order_book.ask(ask2);
    ob::Order ask3{3, ob::OrderSide::SELL, 6, 105};
    order_book.ask(ask3);
    ASSERT_EQ(order_book.asks[110].size(), 2);
    ASSERT_EQ(order_book.asks[105].size(), 1);

    ob::Order bid1{4, ob::OrderSide::BUY, 4, 100};
    order_book.bid(bid1);
    ob::Order bid2{5, ob::OrderSide::BUY, 6, 100};
    order_book.bid(bid2);
    ob::Order bid3{6, ob::OrderSide::BUY, 10, 90};
    order_book.bid(bid3);
    ob::Order bid4{7, ob::OrderSide::BUY, 2, 90};
    order_book.bid(bid4);
    ob::Order bid5{8, ob::OrderSide::BUY, 3, 90};
    order_book.bid(bid5);
    ASSERT_EQ(order_book.bids[100].size(), 2);
    ASSERT_EQ(order_book.bids[90].size(), 3);

    ob::Order ask_should_execute{9, ob::OrderSide::SELL, 23, 80};
    order_book.ask(ask_should_execute);
}

TEST_F(OrderBookTest, TestAskExecutionLeftovers) {
    /**
     * =================
     * ASK
     * 110: 5 10
     * 105: 6
     * ------------
     * 90: 2
     * BID
     * =================
     */

    ob::Order ask1{1, ob::OrderSide::SELL, 5, 110};
    order_book.ask(ask1);
    ob::Order ask2{2, ob::OrderSide::SELL, 10, 110};
    order_book.ask(ask2);
    ob::Order ask3{3, ob::OrderSide::SELL, 6, 105};
    order_book.ask(ask3);

    auto expected_asks = ob::Asks{
        {110, {ask1, ask2}},  // we should have these 2 orders at the same limit
        {105, {ask3}}         // and 1 order here
    };
    ASSERT_EQ(order_book.asks, expected_asks);

    /**
     * Now let’s say we have got a new buy order of 8 shares at the price 107
     USD.
     * We will see trade: 6 shares of XYZ were sold at 105 USD
     * We still have 2 more shares from a buyer who is willing to buy no higher
     * than 107 USD but the best sell order right now is at 110 USD so we place
     a
     * new order at the level of 107 USD and the order book will look like:
     * =================
     * ASK
     * 110: 5 10
     * ------------
     * 107: 2
     * 90: 2
     * BID
     * =================
     */

    ob::Order bid1{4, ob::OrderSide::BUY, 2, 90};
    order_book.bid(bid1);
    ASSERT_EQ(order_book.bids[90].size(), 1);

    auto expected_bids = ob::Bids{{90, {bid1}}};
    ASSERT_EQ(order_book.bids, expected_bids);

    ob::Order bid_should_execute{5, ob::OrderSide::BUY, 8, 107};
    order_book.bid(bid_should_execute);
    ASSERT_EQ(order_book.bids[107].size(), 1);

    auto expected_remaining_bids = ob::Bids{
        {107, {{5, ob::OrderSide::BUY, 2, 107}}},  // leftover bid at this limit
        {90, {bid1}}  // this one was left untouched
    };
    ASSERT_EQ(order_book.bids, expected_remaining_bids);

    auto expected_remaining_asks = ob::Asks{
        {110, {ask1, ask2}},  // These 2 should not have been modified
    };
    ASSERT_EQ(order_book.asks, expected_remaining_asks);
}
}  // namespace obt