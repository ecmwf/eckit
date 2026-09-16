// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File Length.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef eckit_Length_h
#define eckit_Length_h

#include <iosfwd>
#include <vector>

#include "eckit/persist/Bless.h"

namespace eckit {
//-----------------------------------------------------------------------------

class Bless;
class DumpLoad;
class Stream;

// But because the compiler aligns long longs
// on 64bits boundaries and longs on 32 bits boundaries,
// we need the help of a little pragma here, to make ObjectStore happy

#ifdef _AIX
#pragma options align = twobyte
#endif

class Length {
public:  // types

    using value_t = long long;

public:  // methods

    friend std::ostream& operator<<(std::ostream& s, const Length& x);

    friend Stream& operator<<(Stream& s, const Length& x);

    friend Stream& operator>>(Stream& s, Length& x);

    // -- Contructors

    Length(long long l = 0) : value_(l) {}
    Length(const Length& other) : value_(other.value_) {}

#include "eckit/io/Length.b"

public:  // operators

    Length& operator=(const Length& other) {
        value_ = other.value_;
        return *this;
    }

    Length operator+(const Length& other) const { return Length(value_ + other.value_); }

    Length& operator+=(const Length& other) {
        value_ += other.value_;
        return *this;
    }

    bool operator==(const Length& other) const { return value_ == other.value_; }

    bool operator<(const Length& other) const { return value_ < other.value_; }

    operator long long() const { return value_; }

    long long operator-(const Length& other) const { return value_ - other.value_; }

    void dump(DumpLoad&) const;
    void load(DumpLoad&);

private:  // members

    value_t value_;

    friend class Offset;
};

using LengthList = std::vector<Length>;


#ifdef _AIX
#pragma options align = reset
#endif

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
