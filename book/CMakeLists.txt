add_executable(book main.cpp)
target_link_libraries(book fmt::fmt)

add_executable(bookTests tests.cpp)
target_link_libraries(bookTests PRIVATE gtest_main)
gtest_discover_tests(bookTests)