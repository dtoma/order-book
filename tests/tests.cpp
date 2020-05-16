#include "gtest/gtest.h"
#include "order_book.h"

namespace {
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

    bid(order_book, order_buy);
    ASSERT_EQ(order_book.bids[order_buy.price].front(), order_buy);

    ask(order_book, order_sell);
    ASSERT_EQ(order_book.asks[order_sell.price].front(), order_sell);
}

TEST(TestOrderBook, TestBidExecutionSimpleMatch) {
    OrderBook order_book;
    ASSERT_TRUE(order_book.bids.empty());
    ASSERT_TRUE(order_book.asks.empty());

    Order order_buy{1, OrderSide::BUY, 10, 5};
    Order order_sell{2, OrderSide::SELL, 10, 5};

    ask(order_book, order_sell);
    ASSERT_EQ(order_book.asks[order_sell.price].front(), order_sell);

    bid(order_book, order_buy);
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

    ask(order_book, Order{1, OrderSide::SELL, 5, 110});
    ask(order_book, Order{2, OrderSide::SELL, 10, 110});
    ask(order_book, Order{3, OrderSide::SELL, 3, 105});
    ask(order_book, Order{4, OrderSide::SELL, 7, 105});
    ASSERT_EQ(order_book.asks[110].size(), 2);
    ASSERT_EQ(order_book.asks[105].size(), 2);

    bid(order_book, Order{5, OrderSide::BUY, 4, 105});
    ASSERT_TRUE(order_book.bids.empty());
    ASSERT_EQ(order_book.asks[105].size(), 1);
    ASSERT_EQ(order_book.asks[105].front().quantity, 6);
}