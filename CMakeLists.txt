cmake_minimum_required(VERSION 3.8)
project(OrderBook)

include(GoogleTest)

# Make testing available
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

# External dependencies
add_subdirectory(dependencies)

# Internal projects
add_subdirectory(book)
add_subdirectory(tests)