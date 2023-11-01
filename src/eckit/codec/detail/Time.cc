/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/codec/detail/Time.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <ios>       // std::ios_base
#include <iterator>  // std::ostreambuf_iterator
#include <locale>    // std::use_facet, std::time_put
#include <ostream>   // std::basic_ostream
#include <sstream>
#include <string>

#include "eckit/log/JSON.h"

namespace eckit::codec {

namespace {

std::time_t to_time_t(Time time) {
    return static_cast<std::time_t>(time.tv_sec);
}

Time from_time_point(std::chrono::time_point<std::chrono::system_clock> t) {
    auto since_epoch      = t.time_since_epoch();
    auto sec_since_epoch  = std::chrono::duration_cast<std::chrono::seconds>(since_epoch);
    auto nsec_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(since_epoch);
    auto extra_nsec       = std::chrono::duration_cast<std::chrono::nanoseconds>(nsec_since_epoch - sec_since_epoch);

    Time time;
    time.tv_sec  = static_cast<std::uint64_t>(sec_since_epoch.count());
    time.tv_nsec = static_cast<std::uint64_t>(extra_nsec.count());
    return time;
}

}  // namespace


Time Time::now() {
    return from_time_point(std::chrono::system_clock::now());
}

void Time::print(std::ostream& out) const {
    // Will print time-date in ISO 8601 format: 1970-01-01T00:00:00.123456789Z
    auto time = to_time_t(*this);
    out << std::put_time(::gmtime(&time), "%FT%T") << "." << tv_nsec << "Z";
}

std::ostream& operator<<(std::ostream& out, const Time& time) {
    time.print(out);
    return out;
}

JSON& operator<<(JSON& out, const Time& time) {
    std::stringstream s;
    s << time;
    out << s.str();
    return out;
}

std::string Time::str() const {
    std::stringstream s;
    print(s);
    return s.str();
}

}  // namespace eckit::codec
