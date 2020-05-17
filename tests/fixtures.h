#pragma once

#include "gtest/gtest.h"
#include "order_book.h"
#include "spdlog/spdlog.h"

namespace obt {
class OrderBookTest : public ::testing::Test {
   protected:
    void SetUp() override {
        spdlog::set_level(spdlog::level::debug);
        ASSERT_TRUE(this->order_book.bids.empty());
        ASSERT_TRUE(this->order_book.asks.empty());
    }
    // void TearDown() override {}

    ob::OrderBook order_book;
};

}  // namespace obt