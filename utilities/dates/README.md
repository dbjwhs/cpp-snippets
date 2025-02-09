# Setting up C++ Timezone Library Project

## Prerequisites

### Howard Hinnant's Date Library
The date library provides modern C++ date and time zone functionality. It's not part of the standard library, so we need to build it from source:

```bash
git clone https://github.com/HowardHinnant/date.git
cd date
mkdir build
cd build
cmake -DBUILD_TZ_LIB=ON ..
make
sudo make install
```

The `-DBUILD_TZ_LIB=ON` flag is crucial as it enables the timezone functionality.

### CURL Installation
The date library uses CURL to download timezone data. On macOS:
```bash
brew install curl
```

## Project Setup

### Main Code (main.cpp)
```cpp
#include <iostream>
#include <chrono>
#include <date/date.h>
#include <date/tz.h>

using namespace std;
using namespace date;

int main() {
    auto meet_nyc = date::sys_days{date::year{2016}/5/1} + std::chrono::hours{9};
    auto nyc_zone = date::current_zone()->to_local(meet_nyc);
    auto pac_zone = date::locate_zone("America/Los_Angeles")->to_local(meet_nyc);
    auto lon_zone = date::locate_zone("Europe/London")->to_local(meet_nyc);
    auto syd_zone = date::locate_zone("Australia/Sydney")->to_local(meet_nyc);

    auto pac_diff = duration_cast<std::chrono::hours>(pac_zone.time_since_epoch() - 
                   nyc_zone.time_since_epoch());
    auto lon_diff = duration_cast<std::chrono::hours>(lon_zone.time_since_epoch() - 
                   nyc_zone.time_since_epoch());
    auto syd_diff = duration_cast<std::chrono::hours>(syd_zone.time_since_epoch() - 
                   nyc_zone.time_since_epoch());

    cout << "The New York meeting is " << nyc_zone << '\n';
    cout << "The Pacific  meeting is " << pac_zone << " (" << abs(pac_diff.count()) 
         << " hours " << (pac_diff.count() < 0 ? "behind" : "ahead") << ")\n";
    cout << "The London   meeting is " << lon_zone << " (" << abs(lon_diff.count()) 
         << " hours " << (lon_diff.count() < 0 ? "behind" : "ahead") << ")\n";
    cout << "The Sydney   meeting is " << syd_zone << " (" << abs(syd_diff.count()) 
         << " hours " << (syd_diff.count() < 0 ? "behind" : "ahead") << ")\n";

    return 0;
}
```

### CMake Configuration (CMakeLists.txt)
```cmake
cmake_minimum_required(VERSION 3.15)
project(untitled1)

set(CMAKE_CXX_STANDARD 17)

find_package(CURL REQUIRED)
find_path(DATE_INCLUDE_DIR date/date.h)
find_library(DATE_LIBRARY date-tz)

add_executable(untitled1 main.cpp)
target_link_libraries(untitled1 PRIVATE ${DATE_LIBRARY} CURL::libcurl)
target_include_directories(untitled1 PRIVATE ${DATE_INCLUDE_DIR})
```

## Why CURL is Required
The date library uses CURL to:
1. Download timezone database files (IANA Time Zone Database)
2. Update timezone information when needed
3. Handle remote timezone data fetching

## Building the Project
```bash
mkdir build
cd build
cmake ..
make
```

## Common Issues
- Missing CURL: Install via brew on macOS
- Architecture issues: Ensure CURL is installed for the correct architecture (arm64 for M1/M2 Macs)
- Library path issues: May need to set LIBRARY_PATH to include homebrew libraries
```bash
export LIBRARY_PATH=$LIBRARY_PATH:/opt/homebrew/lib
```
