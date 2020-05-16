#include "gtest/gtest.h"
#include "order_book.h"

TEST(TestString, TestDefaultConstructor) {
    Order order_1{100000, OrderSide::SELL, 1, 1075};
    Order order_2{100001, OrderSide::BUY, 9, 1000};
    Order order_3{100002, OrderSide::BUY, 30, 975};
    Order order_4{100003, OrderSide::SELL, 10, 1050};
    Order order_5{100004, OrderSide::BUY, 10, 950};
    Order order_6{100005, OrderSide::SELL, 2, 1025};
    Order order_7{100006, OrderSide::BUY, 1, 1000};
    Order order_8{100007, OrderSide::SELL, 5, 1025};
    Order order_9{100008, OrderSide::BUY, 3, 1050};

    ASSERT_TRUE(true);
}