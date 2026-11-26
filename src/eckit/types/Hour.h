// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @date   May 21


#ifndef eckit_seconds_h
#define eckit_seconds_h

#include <iosfwd>
#include <string>

namespace eckit {

//--------------------------------------------------------------------------------------------------


class Hour {
public:

    // -- Contructors

    explicit Hour(double hour);
    explicit Hour(const std::string&);

    // -- Copy

    // -- Methods

    std::string asString() const;

    // -- Convertors

    operator std::string() const { return asString(); }

    // -- Operators

    bool operator==(const Hour& other) const { return seconds_ == other.seconds_; }

    bool operator!=(const Hour& other) const { return seconds_ != other.seconds_; }

    bool operator<(const Hour& other) const { return seconds_ < other.seconds_; }

    bool operator>(const Hour& other) const { return seconds_ > other.seconds_; }

    bool operator<=(const Hour& other) const { return seconds_ <= other.seconds_; }

    bool operator>=(const Hour& other) const { return seconds_ >= other.seconds_; }


    // -- Methods


    // -- Class methods


    // -- Friends

    friend std::ostream& operator<<(std::ostream& s, const Hour& hour) {
        hour.print(s);
        return s;
    }


private:

    // -- Members

    long seconds_ = 0;

    // -- Methods

    void print(std::ostream&) const;

    // -- Class methods

    // -- Friends
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
