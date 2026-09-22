// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <iosfwd>
#include <string>

namespace eckit {
class JSON;
}
namespace eckit::codec {

/// Store UTC time up to nanosecond precision
struct Time {
    std::uint64_t tv_sec{0};   ///<    seconds since Epoch (1970-01-01T00:00:00Z)
    std::uint64_t tv_nsec{0};  ///<    additional nanoseconds

    /// Create current time using system clock
    static Time now();

    /// print UTC time in ISO 8601 format: "1970-01-01T00:00:00.123456789Z"
    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream&, const Time&);
    friend JSON& operator<<(JSON&, const Time&);

    /// @return string of UTC time in ISO 8601 format: "1970-01-01T00:00:00.123456789Z"
    std::string str() const;
};


}  // namespace eckit::codec
