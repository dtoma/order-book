#include "gtest/gtest.h"
#include "order_book.h"

using namespace ob;

namespace {
// TODO: Fixtures
Order order_1{100000, OrderSide::SELL, 1, 1075};
Order order_2{100001, OrderSide::BUY, 9, 1000};
Order order_3{100002, OrderSide::BUY, 30, 975};
Order order_4{100003, OrderSide::SELL, 10, 1050};
Order order_5{100004, OrderSide::BUY, 10, 950};
Order order_6{100005, OrderSide::SELL, 2, 1025};
Order order_7{100006, OrderSide::BUY, 1, 1000};
Order order_8{100007, OrderSide::SELL, 5, 1025};
Order order_9{100008, OrderSide::BUY, 3, 1050};
}  // namespace

TEST(TestOrderBook, TestAddOrders) {
    OrderBook order_book;

    Order order_buy{1, OrderSide::BUY, 10, 5};
    Order order_sell{2, OrderSide::SELL, 10, 10};

    order_book.bid(order_buy);
    ASSERT_EQ(order_book.bids[order_buy.price].front(), order_buy);

    order_book.ask(order_sell);
    ASSERT_EQ(order_book.asks[order_sell.price].front(), order_sell);
}

TEST(TestOrderBook, TestBidExecutionSimpleMatch) {
    OrderBook order_book;
    ASSERT_TRUE(order_book.bids.empty());
    ASSERT_TRUE(order_book.asks.empty());

    Order order_buy{1, OrderSide::BUY, 10, 5};
    Order order_sell{2, OrderSide::SELL, 10, 5};

    order_book.ask(order_sell);
    ASSERT_EQ(order_book.asks[order_sell.price].front(), order_sell);

    order_book.bid(order_buy);
    ASSERT_TRUE(order_book.bids.empty());
    ASSERT_TRUE(order_book.asks.empty());
}

/**
 * This is one of the scenarios from the email:

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
 * We will see second trade happening:

 * 1 share of XYZ were sold at 105 USD
 */
TEST(TestOrderBook, TestBidExecutionMultipleMatches) {
    OrderBook order_book;
    ASSERT_TRUE(order_book.bids.empty());
    ASSERT_TRUE(order_book.asks.empty());

    Order ask1{1, OrderSide::SELL, 5, 110};
    order_book.ask(ask1);
    Order ask2{2, OrderSide::SELL, 10, 110};
    order_book.ask(ask2);
    Order ask3{3, OrderSide::SELL, 3, 105};
    order_book.ask(ask3);
    Order ask4{4, OrderSide::SELL, 7, 105};
    order_book.ask(ask4);
    ASSERT_EQ(order_book.asks[110].size(), 2);
    ASSERT_EQ(order_book.asks[105].size(), 2);

    Order bid1{5, OrderSide::BUY, 4, 105};
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
TEST(TestOrderBook, TestAskExecutionMultipleMatches) {
    OrderBook order_book;
    ASSERT_TRUE(order_book.bids.empty());
    ASSERT_TRUE(order_book.asks.empty());

    Order ask1{1, OrderSide::SELL, 5, 110};
    order_book.ask(ask1);
    Order ask2{2, OrderSide::SELL, 10, 110};
    order_book.ask(ask2);
    Order ask3{3, OrderSide::SELL, 6, 105};
    order_book.ask(ask3);
    ASSERT_EQ(order_book.asks[110].size(), 2);
    ASSERT_EQ(order_book.asks[105].size(), 1);

    Order bid1{4, OrderSide::BUY, 4, 100};
    order_book.bid(bid1);
    Order bid2{5, OrderSide::BUY, 6, 100};
    order_book.bid(bid2);
    Order bid3{6, OrderSide::BUY, 10, 90};
    order_book.bid(bid3);
    Order bid4{7, OrderSide::BUY, 2, 90};
    order_book.bid(bid4);
    Order bid5{8, OrderSide::BUY, 3, 90};
    order_book.bid(bid5);
    ASSERT_EQ(order_book.bids[100].size(), 2);
    ASSERT_EQ(order_book.bids[90].size(), 3);

    Order ask_should_execute{9, OrderSide::SELL, 23, 80};
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
 * We will see trade:
 * 6 shares of XYZ were sold at 105 USD:

 * =================
 * ASK
 * 110: 5 10
 * ------------
 * 90: 2
 * BID
 * =================

 * We still have 2 more shares from a buyer who is willing to buy no higher than
 * 107 USD but the best sell order right now is at 110 USD so we place a new
 order
 * at the level of 107 USD and the order book will look like:

 * =================
 * ASK
 * 110: 5 10
 * ------------
 * 107: 2
 * 90: 2
 * BID
 * =================
 */
TEST(TestOrderBook, TestAskExecutionLeftovers) {
    OrderBook order_book;
    ASSERT_TRUE(order_book.bids.empty());
    ASSERT_TRUE(order_book.asks.empty());

    Order ask1{1, OrderSide::SELL, 5, 110};
    order_book.ask(ask1);
    Order ask2{2, OrderSide::SELL, 10, 110};
    order_book.ask(ask2);
    Order ask3{3, OrderSide::SELL, 6, 105};
    order_book.ask(ask3);
    ASSERT_EQ(order_book.asks[110].size(), 2);
    ASSERT_EQ(order_book.asks[105].size(), 1);

    Order bid1{4, OrderSide::BUY, 2, 90};
    order_book.bid(bid1);
    ASSERT_EQ(order_book.bids[90].size(), 1);

    Order bid_should_execute{5, OrderSide::BUY, 8, 107};
    order_book.bid(bid_should_execute);
    ASSERT_EQ(order_book.bids[107].size(), 1);
}