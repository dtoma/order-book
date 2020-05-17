#include "fixtures.h"

namespace obt {

TEST_F(OrderBookTest, TestCancelAsk) {
    ob::Order ask1{1, ob::OrderSide::SELL, 5, 110};
    order_book.ask(ask1);
    ASSERT_EQ(order_book.asks[110].size(), 1);
    order_book.cancel(ask1);
    ASSERT_TRUE(order_book.asks.empty());
}

TEST_F(OrderBookTest, TestCancelBid) {
    ob::Order bid1{1, ob::OrderSide::BUY, 5, 110};
    order_book.bid(bid1);
    ASSERT_EQ(order_book.bids[110].size(), 1);
    order_book.cancel(bid1);
    ASSERT_TRUE(order_book.bids.empty());
}
}  // namespace obt