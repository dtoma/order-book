add_executable(order_book_bench bench.cpp)

target_link_libraries(order_book_bench PRIVATE benchmark::benchmark order_book)

add_custom_target(runbench COMMAND order_book_bench)