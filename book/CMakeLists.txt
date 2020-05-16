add_library(order_book STATIC src/order_book.cpp include/order_book.h)

target_link_libraries(order_book fmt::fmt)
target_include_directories(order_book PUBLIC include)

if(MSVC)
  target_compile_options(order_book PRIVATE /W4 /WX)
else()
  target_compile_options(order_book PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
