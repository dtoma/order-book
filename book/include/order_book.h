#pragma once

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "spdlog/fmt/ostr.h"

//! Declaration & implementation of the order book library
namespace ob {

/** \brief Define a new type to represent prices.
 *
 * Ideally this would be a strong typedef.
 */
using Price = int;

/** \brief Define a new type to represent quantities.
 *
 * Ideally this would be a strong typedef.
 */
using Quantity = int;

//! Strongly-typed order side.
enum class OrderSide { BUY, SELL };

//! Strongly-typed order type.
enum class OrderType { NEW, CANCEL };

//! For larger enums, x-macros would be useful.
static const std::map<std::string, OrderType> StringToOrderType{
    {"A", OrderType::NEW}, {"X", OrderType::CANCEL}};

//! POD order representation
struct Order final {
    int id;             //!< Unique ID
    OrderSide side;     //!< Buy/Sell
    Quantity quantity;  //!< Remaining quantity
    Price price;        //!< Price
};

//! Enable use of ASSERT_EQ on Order objects in the test suite.
inline bool operator==(Order const &left, Order const &right) {
    return left.id == right.id && left.side == right.side &&
           left.quantity == right.quantity && left.price == right.price;
}

/** \brief Using std:map with default ordering, to hit the smallest asks first
 *         when we place a bid.
 *
 * We could easily reserve a few limits to reduce the number of allocations.
 */
using Asks = std::map<Price, std::vector<Order>>;

/** \brief Using std:map with default ordering, to hit the largest bids first
 *         when we place an ask.
 *
 * We could easily reserve a few limits to reduce the number of allocations.
 */
using Bids = std::map<Price, std::vector<Order>, std::greater<Price>>;

//! Bid and ask tables, and functions to add/cancel orders.
struct OrderBook final {
    Bids bids;  //! Table of bids
    Asks asks;  //! Table of asks

    void show_bids() const;  //! Log the current bids table
    void show_asks() const;  //! Log the current bids table

    //! Execute an order as much as possible at a given limit.
    auto execute_at_limit(std::vector<Order> &, Order &);

    /** \brief Try to add a bid order.
     *
     * Try to execute the order as much as possible, then place the remaining
     * order in the bid table.
     */
    void bid(Order &);

    /** \brief Try to add a ask order.
     *
     * Try to execute the order as much as possible, then places the remaining
     * order in the ask table.
     */
    void ask(Order &);

    //! Remove an order if it is still in the book, using its ID.
    void cancel(Order const &);

    //! Call bid or ask depending on the order's side.
    void place_order(Order &);
};  // namespace ob

//! Deserialize text into Order objects
std::vector<std::pair<OrderType, Order>> read_orders_file(std::ifstream &);

}  // namespace ob