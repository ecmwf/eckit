// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>

#include "eckit/types/SemanticVersion.h"

namespace eckit::codec {

struct Version {             // 8 bytes
    std::uint32_t major{0};  ///<  Major version
    std::uint32_t minor{2};  ///<  Minor version

    std::string str() const { return std::to_string(major) + "." + std::to_string(minor); }
    operator std::string() const { return str(); }
    operator SemanticVersion() const { return SemanticVersion{major, minor, 0}; }

    bool operator<(const Version& v) const {
        return SemanticVersion{major, minor, 0} < SemanticVersion{v.major, v.minor, 0};
    }
    bool operator==(const Version& v) const {
        return SemanticVersion{major, minor, 0} == SemanticVersion{v.major, v.minor, 0};
    }
    bool operator!=(const Version& v) const { return !(*this == v); }
    bool operator<=(const Version& v) const { return (*this < v) or (*this == v); }
    bool operator>(const Version& v) const { return !(*this <= v); }
    bool operator>=(const Version& v) const { return (*this > v) or (*this == v); }


    friend std::ostream& operator<<(std::ostream& out, const Version& v) {
        out << v.str();
        return out;
    }
};


}  // namespace eckit::codec
