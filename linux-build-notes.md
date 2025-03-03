# C++ Build Issues - Summary of Fixes (3/3/2025)

These sub project where originally developed and built on OS X, however I choose to use CMake to do the build so that these could be built on multiple operating systems, specifically my HP  DevOne Pop OS (ubuntu 22.04) laptop. There was a bit of work to get everyone project building. For my future self I kept notes, which I am saving in the root of the cpp-snippets repro. I fully plan to perform this same effort for Window (version something).

## 1. Missing Header Includes

- `std::clamp()` → Needed `#include <algorithm>`
- `std::find()` → Needed `#include <algorithm>`
- `std::function` → Needed `#include <functional>`
- `std::unordered_map` → Needed `#include <unordered_map>`

## 2. C++20 Ranges Implementation Issues

Several projects using C++20 ranges features encountered compiler errors:
- `std::ranges::for_each`, `std::ranges::partial_sort`, etc. would fail even with `#include <ranges>`
- The core issue: ranges algorithms were trying to use standard algorithms but couldn't find them
- Solutions:
  - Include `<algorithm>` **before** `<ranges>`
  - Fall back to traditional algorithm versions (`std::for_each` instead of `std::ranges::for_each`)
  - Downgrade to C++17 if ranges features aren't critical

## 3. Language Feature Limitations

I encountered some cases where C++ language features were being used inappropriately:
- `constexpr std::string` isn't allowed because `std::string` isn't constexpr-friendly
- Solution: Use `const std::string` or `constexpr std::string_view` instead
- Open question why this worked on OS X?

## 4. External Library Dependencies

Some projects required external libraries:
- CURL → Fixed by installing `sudo apt install libcurl4-openssl-dev`
- Howard Hinnant's date library → Needed manual installation from source with timezone support

```bash
cd ~
git clone https://github.com/HowardHinnant/date.git
cd date
mkdir build && cd build
cmake .. -DBUILD_TZ_LIB=ON -DUSE_SYSTEM_TZ_DB=ON
make
sudo make install
```

## Insights

1. **Be cautious with C++20 features**: Some features might not be fully supported in all environments.

2. **Check compiler compatibility**: Your code works in CLion but not from the command line because of environment differences or includes being handled differently.

3. **Language limitations**: Not all constructs are allowed in all contexts, such as constexpr with std::string.

4. **External dependencies**: Always document external libraries your projects need for easier setup.

For your C++17/C++20 projects, make sure both the appropriate standard is set in CMakeLists.txt and that all required headers are included explicitly in your source files.
