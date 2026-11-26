// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Tiago Quintino
/// @date Jan 2021

#pragma once

#include <iosfwd>
#include <string>

namespace eckit {

//--------------------------------------------------------------------------------------------------

/// Represents a Semantic Version with correct version as a POD
///
class SemanticVersion {
public:  // methods

    SemanticVersion();
    SemanticVersion(unsigned long major, unsigned long minor, unsigned long patch);
    SemanticVersion(const std::string&);

    operator std::string() const;

    bool operator<(const SemanticVersion&) const;
    bool operator==(const SemanticVersion&) const;
    bool operator!=(const SemanticVersion& o) const { return !(*this == o); }
    bool operator<=(const SemanticVersion& o) const { return (*this < o) or (*this == o); }
    bool operator>(const SemanticVersion& o) const { return !(*this <= o); }
    bool operator>=(const SemanticVersion& o) const { return (*this > o) or (*this == o); }

private:  // methods

    void print(std::ostream& s) const { s << std::string(*this); }

    friend std::ostream& operator<<(std::ostream& os, const SemanticVersion& u) {
        u.print(os);
        return os;
    }

private:  // members

    unsigned long major_;
    unsigned long minor_;
    unsigned long patch_;
};

//--------------------------------------------------------------------------------------------------

}  // namespace eckit
