# C++20 Cooperative Cancellation with `std::stop_token` on macOS

This project demonstrates the use of the C++20 cooperative cancellation mechanism (using `std::stop_token`, `std::stop_source`, `std::stop_callback`, and `std::jthread`) on macOS.

## Status on macOS

As of macOS with Apple Clang 17.0.0, the cooperative cancellation mechanism is available but requires the `-fexperimental-library` compiler flag. This flag has been added to our CMakeLists.txt.

```cmake
# Add the experimental library flag for macOS
if(APPLE)
    add_compile_options(-fexperimental-library)
endif()
```

## Key Components

The cooperative cancellation pattern introduced in C++20 consists of:

1. `std::stop_token`: Receives the cancellation state.
2. `std::stop_source`: Issues cancellation requests.
3. `std::stop_callback`: Executes callbacks when cancellation occurs.
4. `std::jthread`: A thread that automatically propagates cancellation and joins on destruction.

## Sample Code

This project demonstrates:

- Basic stop token functionality
- Using jthread with a stop token for automatic cancellation
- Using stop token with a regular thread
- Registering callbacks with an already stopped token

## Building and Running

```
mkdir -p build
cd build
cmake ..
cmake --build .
./stop_token_source_callback
```

## Notes

- The `stop_requested()` check in a loop allows for cooperative cancellation.
- The `stop_callback` mechanism allows for code to be executed when cancellation is requested.
- `std::jthread` automatically propagates a stop token to the thread function and joins on destruction.
- This functionality is part of the C++20 standard but is marked as experimental in Apple's implementation.

## Usage Benefits

The cooperative cancellation pattern provides several advantages:

1. Safer thread termination than forcible cancellation
2. Cleaner code with less boilerplate compared to manual mechanisms
3. Composable cancellation through token passing
4. Automatic cleanup with `std::jthread`
5. Callback registration for cancellation events

## References

- [C++20 Standard](https://www.iso.org/standard/79358.html)
- [P0660R10: Stop Token and Joining Thread](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0660r10.pdf)
- [Anthony Williams C++20 Concurrency Talk](https://www.youtube.com/watch?v=A7sVFJLJM-A)
