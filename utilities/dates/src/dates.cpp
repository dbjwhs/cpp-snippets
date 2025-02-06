// MIT License
// Copyright (c) 2025 dbjwhs

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
//   auto sum = h1 + m1; // Safe, automatic unit conversion
//
// Much of this work became part of C++20's standard library, though his original date
// library provides additional features beyond the standard.
// This function demonstrates timezone conversions using Howard Hinnant's date library
// It shows how to convert a meeting time in New York to different time zones
void howard_hinnant_date_snippets() {
    // create a time point for May 1st, 2016 at 9:00 AM UTC
    // sys_days converts the calendar date to a time point, then add 9 hours
    const auto meet_nyc = date::sys_days{date::year{2016}/5/1} + std::chrono::hours{9};

    // convert the UTC time to New York local time using the system's current timezone
    const auto nyc_zone = date::current_zone()->to_local(meet_nyc);

    // convert the same UTC time point to different timezone local times
    // locate_zone finds the timezone database entry for each region
    const auto pac_zone = date::locate_zone("America/Los_Angeles")->to_local(meet_nyc);
    const auto lon_zone = date::locate_zone("Europe/London")->to_local(meet_nyc);
    const auto syd_zone = date::locate_zone("Australia/Sydney")->to_local(meet_nyc);

    // calculate time differences between each zone and NYC
    // time_since_epoch() gives duration since Unix epoch (1970-01-01)
    // duration_cast converts the difference to hours
    const auto pac_diff = duration_cast<std::chrono::hours>(
        pac_zone.time_since_epoch() - nyc_zone.time_since_epoch());
    const auto lon_diff = duration_cast<std::chrono::hours>(
        lon_zone.time_since_epoch() - nyc_zone.time_since_epoch());
    const auto syd_diff = duration_cast<std::chrono::hours>(
        syd_zone.time_since_epoch() - nyc_zone.time_since_epoch());

    // output the meeting times in each timezone
    // the abs() function is used to show absolute hour differences
    // the ternary operator determines if the timezone is ahead or behind NYC
    cout << "The New York meeting is " << nyc_zone << '\n';
    cout << "The Pacific  meeting is " << pac_zone << " (" << abs(pac_diff.count())
         << " hours " << (pac_diff.count() < 0 ? "behind" : "ahead") << ")\n";
    cout << "The London   meeting is " << lon_zone << " (" << abs(lon_diff.count())
         << " hours " << (lon_diff.count() < 0 ? "behind" : "ahead") << ")\n";
    cout << "The Sydney   meeting is " << syd_zone << " (" << abs(syd_diff.count())
         << " hours " << (syd_diff.count() < 0 ? "behind" : "ahead") << ")\n";

    // example output
    // The New York meeting is 2016-05-01 02:00:00
    // The Pacific  meeting is 2016-05-01 02:00:00 (0 hours ahead)
    // The London   meeting is 2016-05-01 10:00:00 (8 hours ahead)
    // The Sydney   meeting is 2016-05-01 19:00:00 (17 hours ahead)
}

int main() {
    howard_hinnant_date_snippets();
    return 0;
}