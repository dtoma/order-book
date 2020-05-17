# Order Book

![](https://github.com/dtoma/order-book/workflows/CI/badge.svg)

## Build

The project is built using CMake, so we can build/test/run with portable commands.

Example commands can be found in [the CI configuration file](.github/workflows/cpp.yml).

## Format

The code is formatted automatically using `clang-format`.

## Documentation

A HTML documentation is generated using `doxygen`.

```
$> cd ./docs/html/
$> python -m http.server
```

## Benchmarks

Some simple benchmarks are included for basic scenarios.

## Project organization

The order book is implemented as a library. The main executables link against this library.

```
              +---------------+
              |               +^----------------------+
          +--->  Order Book   <-------+               |
          |   |               |       |               |
          |   +---------------+       |               |
          |                           |               |
          |                           |               |
          |                           |               |
+---------+---+               +-------+--------+      |
|             |               |                |      |
|    Main     |               |     Tests      |   +--+-------------+
|             |               |                |   |                |
+-------------+               +----------------+   |   Benchmarks   |
                                                   |                |
                                                   +----------------+
```

## Potential improvements

- Use the order of the map keys to hit the right limits faster instead of iterating too much
- Send events when trades/cancels happen
- Enforce more invariants eg. unique order IDs