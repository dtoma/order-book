# The library

add_library(order_book STATIC src/order_book.cpp include/order_book.h)

if(MSVC)
  target_compile_options(order_book PRIVATE /W4 /WX)
else()
  target_compile_options(order_book PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

target_link_libraries(order_book fmt::fmt spdlog::spdlog)
target_include_directories(order_book PUBLIC include)


# The executable

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE order_book)

if(MSVC)
  target_compile_options(main PRIVATE /W4 /WX)
else()
  target_compile_options(main PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

add_custom_target(runbook COMMAND main ${CMAKE_SOURCE_DIR}/orders.csv)