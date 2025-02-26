// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <chrono>
#include <date/date.h>
#include <date/tz.h>

using namespace std;
using namespace date;
using std::chrono::duration_cast;

// howard hinnant is a key contributor to c++ date/time handling, primarily through the
// <chrono> library (c++11) and his date library. his major contributions include:
//
// 1. the <chrono> library design:
// - created type-safe duration types (hours, minutes, seconds) with compile-time unit
//   conversion and arithmetic
// - implemented clear separation between time points and durations
// - designed comprehensive clock abstractions (system_clock, steady_clock,
//   high_resolution_clock)
//
// 2. the date library:
// - extended <chrono> with calendar functionality (later standardized in c++20)
// - added year, month, day, and weekday types with full support for calendar
//   calculations
// - implemented robust date parsing and formatting
// - handled complex cases like leap years, varying month lengths, timezone
//   conversions, and daylight saving time
//
// his innovative approach prioritized compile-time safety through strong typing to
// prevent unit mixing errors, while maintaining high runtime performance. a key
// implementation detail uses civil calendar counting (days since 1970-01-01) for
// simplified internal calculations.
//
// example of type-safe duration arithmetic:
//   using namespace std::chrono;
//   hours h1(1);
//   minutes m1(30);
//   auto sum = h1 + m1; // safe, automatic unit conversion
//
// much of this work became part of c++20's standard library, though his original date
// library provides additional features beyond the standard.
// this function demonstrates timezone conversions using howard hinnant's date library
// it shows how to convert a meeting time in new york to different time zones
void howard_hinnant_date_snippets() {
    // create a time point for may 1st, 2016 at 9:00 am utc
    // sys_days converts the calendar date to a time point, then add 9 hours
    const auto meet_nyc = date::sys_days{date::year{2016}/5/1} + std::chrono::hours{9};

    // convert the utc time to new york local time using the system's current timezone
    const auto nyc_zone = date::current_zone()->to_local(meet_nyc);

    // convert the same utc time point to different timezone local times
    // locate_zone finds the timezone database entry for each region
    const auto pac_zone = date::locate_zone("America/Los_Angeles")->to_local(meet_nyc);
    const auto lon_zone = date::locate_zone("Europe/London")->to_local(meet_nyc);
    const auto syd_zone = date::locate_zone("Australia/Sydney")->to_local(meet_nyc);

    // calculate time differences between each zone and nyc
    // time_since_epoch() gives duration since unix epoch (1970-01-01)
    // duration_cast converts the difference to hours
    const auto pac_diff = duration_cast<std::chrono::hours>(
        pac_zone.time_since_epoch() - nyc_zone.time_since_epoch());
    const auto lon_diff = duration_cast<std::chrono::hours>(
        lon_zone.time_since_epoch() - nyc_zone.time_since_epoch());
    const auto syd_diff = duration_cast<std::chrono::hours>(
        syd_zone.time_since_epoch() - nyc_zone.time_since_epoch());

    // output the meeting times in each timezone
    // the abs() function is used to show absolute hour differences
    // the ternary operator determines if the timezone is ahead or behind nyc
    cout << "The New York meeting is " << nyc_zone << '\n';
    cout << "The Pacific  meeting is " << pac_zone << " (" << abs(pac_diff.count())
         << " hours " << (pac_diff.count() < 0 ? "behind" : "ahead") << ")\n";
    cout << "The London   meeting is " << lon_zone << " (" << abs(lon_diff.count())
         << " hours " << (lon_diff.count() < 0 ? "behind" : "ahead") << ")\n";
    cout << "The Sydney   meeting is " << syd_zone << " (" << abs(syd_diff.count())
         << " hours " << (syd_diff.count() < 0 ? "behind" : "ahead") << ")\n";

    // example output
    // the new york meeting is 2016-05-01 02:00:00
    // the pacific  meeting is 2016-05-01 02:00:00 (0 hours ahead)
    // the london   meeting is 2016-05-01 10:00:00 (8 hours ahead)
    // the sydney   meeting is 2016-05-01 19:00:00 (17 hours ahead)
}

int main() {
    howard_hinnant_date_snippets();
    return 0;
}
