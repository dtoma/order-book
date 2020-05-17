#pragma once

#include "fixtures.h"

namespace obt {
TEST_F(OrderBookTest, TestBidExecutionSimpleMatch) {
    ob::Order order_buy{1, ob::OrderSide::BUY, 10, 5};
    ob::Order order_sell{2, ob::OrderSide::SELL, 10, 5};

    order_book.ask(order_sell);
    ASSERT_EQ(order_book.asks[order_sell.price].front(), order_sell);

    order_book.bid(order_buy);
    ASSERT_TRUE(order_book.bids.empty());
    ASSERT_TRUE(order_book.asks.empty());
}

/**
 * =================
 * ASK
 * 110: 5 10
 * 105: 3 7
 * ------------
 * 100: 4 6
 * 90: 10 2 3
 * BID
 * =================

 * Now, let’s imagine some buyer places an “aggressive” order to buy 4 shares at
 * the price of 105 USD. It will be an order which actually matches the best
 * price from sellers (namely lowest price at 105 USD).
 * The seller’s order of 3 shares at price 105 USD was added earlier than
 * another seller’s order of 7 shares at 105 USD.
 * Therefore seller’s order of 3 shares will be matched first.

 * As a result we will see a trade: `3 shares of XYZ were sold at 105 USD`
 * And the order book will look like:

 * =================
 * ASK
 * 110: 5 10
 * 105: 7
 * ------------
 * 100: 4 6
 * 90: 10 2 3
 * BID
 * =================

 * But we still have 1 share left from an “aggressive” buyer who wants to buy at
 * 105 USD. Therefore we will have partial match with seller’s order at 105 USD.
 * We will see second trade happening: 1 share of XYZ were sold at 105 USD
 */
TEST_F(OrderBookTest, TestBidExecutionMultipleMatches) {
    ob::Order ask1{1, ob::OrderSide::SELL, 5, 110};
    order_book.ask(ask1);
    ob::Order ask2{2, ob::OrderSide::SELL, 10, 110};
    order_book.ask(ask2);
    ob::Order ask3{3, ob::OrderSide::SELL, 3, 105};
    order_book.ask(ask3);
    ob::Order ask4{4, ob::OrderSide::SELL, 7, 105};
    order_book.ask(ask4);
    ASSERT_EQ(order_book.asks[110].size(), 2);
    ASSERT_EQ(order_book.asks[105].size(), 2);

    ob::Order bid1{5, ob::OrderSide::BUY, 4, 105};
    order_book.bid(bid1);
    ASSERT_TRUE(order_book.bids.empty());
    ASSERT_EQ(order_book.asks[105].size(), 1);
    ASSERT_EQ(order_book.asks[105].front().quantity, 6);
}

/**
 * =================
 * ASK
 * 110: 5 10
 * 105: 6
 * ------------
 * 100: 4 6
 * 90: 10 2 3
 * BID
 * =================
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

/**
 * =================
 * ASK
 * 110: 5 10
 * 105: 6
 * ------------
 * 90: 2
 * BID
 * =================

 * Now let’s say we have got a new buy order of 8 shares at the price 107 USD.
 * We will see trade: 6 shares of XYZ were sold at 105 USD

 * =================
 * ASK
 * 110: 5 10
 * ------------
 * 90: 2
 * BID
 * =================

 * We still have 2 more shares from a buyer who is willing to buy no higher than
 * 107 USD but the best sell order right now is at 110 USD so we place a new
 * order at the level of 107 USD and the order book will look like:

 * =================
 * ASK
 * 110: 5 10
 * ------------
 * 107: 2
 * 90: 2
 * BID
 * =================
 */
TEST_F(OrderBookTest, TestAskExecutionLeftovers) {
    ob::Order ask1{1, ob::OrderSide::SELL, 5, 110};
    order_book.ask(ask1);
    ob::Order ask2{2, ob::OrderSide::SELL, 10, 110};
    order_book.ask(ask2);
    ob::Order ask3{3, ob::OrderSide::SELL, 6, 105};
    order_book.ask(ask3);
    ASSERT_EQ(order_book.asks[110].size(), 2);
    ASSERT_EQ(order_book.asks[105].size(), 1);

    ob::Order bid1{4, ob::OrderSide::BUY, 2, 90};
    order_book.bid(bid1);
    ASSERT_EQ(order_book.bids[90].size(), 1);

    ob::Order bid_should_execute{5, ob::OrderSide::BUY, 8, 107};
    order_book.bid(bid_should_execute);
    ASSERT_EQ(order_book.bids[107].size(), 1);
}
}  // namespace obt