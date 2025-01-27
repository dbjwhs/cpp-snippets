#include <iostream>
#include <chrono>
#include <date/date.h>
#include <date/tz.h>

using namespace std;
using namespace date;
using std::chrono::duration_cast;

// Howard Hinnant is a key contributor to C++ date/time handling, primarily through the
// <chrono> library (C++11) and his date library. His major contributions include:
//
// 1. The <chrono> Library Design:
// - Created type-safe duration types (hours, minutes, seconds) with compile-time unit
//   conversion and arithmetic
// - Implemented clear separation between time points and durations
// - Designed comprehensive clock abstractions (system_clock, steady_clock,
//   high_resolution_clock)
//
// 2. The Date Library:
// - Extended <chrono> with calendar functionality (later standardized in C++20)
// - Added year, month, day, and weekday types with full support for calendar
//   calculations
// - Implemented robust date parsing and formatting
// - Handled complex cases like leap years, varying month lengths, timezone
//   conversions, and daylight saving time
//
// His innovative approach prioritized compile-time safety through strong typing to
// prevent unit mixing errors, while maintaining high runtime performance. A key
// implementation detail uses civil calendar counting (days since 1970-01-01) for
// simplified internal calculations.
//
// Example of type-safe duration arithmetic:
//   using namespace std::chrono;
//   hours h1(1);
//   minutes m1(30);
//   auto sum = h1 + m1;  // Safe, automatic unit conversion
//
// Much of this work became part of C++20's standard library, though his original date
// library provides additional features beyond the standard.
void howard_hinnant_date_snippets() {
    auto meet_nyc = date::sys_days{date::year{2016}/5/1} + std::chrono::hours{9};
    auto nyc_zone = date::current_zone()->to_local(meet_nyc);
    auto pac_zone = date::locate_zone("America/Los_Angeles")->to_local(meet_nyc);
    auto lon_zone = date::locate_zone("Europe/London")->to_local(meet_nyc);
    auto syd_zone = date::locate_zone("Australia/Sydney")->to_local(meet_nyc);

    auto pac_diff = duration_cast<std::chrono::hours>(pac_zone.time_since_epoch() - nyc_zone.time_since_epoch());
    auto lon_diff = duration_cast<std::chrono::hours>(lon_zone.time_since_epoch() - nyc_zone.time_since_epoch());
    auto syd_diff = duration_cast<std::chrono::hours>(syd_zone.time_since_epoch() - nyc_zone.time_since_epoch());

    cout << "The New York meeting is " << nyc_zone << '\n';
    cout << "The Pacific  meeting is " << pac_zone << " (" << abs(pac_diff.count()) << " hours "
         << (pac_diff.count() < 0 ? "behind" : "ahead") << ")\n";
    cout << "The London   meeting is " << lon_zone << " (" << abs(lon_diff.count()) << " hours "
         << (lon_diff.count() < 0 ? "behind" : "ahead") << ")\n";
    cout << "The Sydney   meeting is " << syd_zone << " (" << abs(syd_diff.count()) << " hours "
         << (syd_diff.count() < 0 ? "behind" : "ahead") << ")\n";
}


int main() {
    howard_hinnant_date_snippets();
    return 0;
}