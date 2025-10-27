/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Offset.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef eckit_Offset_h
#define eckit_Offset_h

#include <iosfwd>
#include <vector>

#include "eckit/io/Length.h"
#include "eckit/persist/Bless.h"

namespace eckit {

//-----------------------------------------------------------------------------

// Forwarded declarations

class Bless;
class Stream;

// But because the compiler aligns long longs
// on 64bits boundaries and longs on 32 bits boundaries,
// we need the help of a little pragma here, to make ObjectStore happy

#ifdef _AIX
#pragma options align = twobyte
#endif

class Offset {
public:  // types

    typedef long long value_t;

public:

    friend std::ostream& operator<<(std::ostream& s, const Offset& x);

    friend Stream& operator<<(Stream& s, const Offset& x);

    friend Stream& operator>>(Stream& s, Offset& x);

    // Offset(fpos_t); <- To implement
    Offset(value_t l = 0) : value_(l) {}
    Offset(const Offset& other) : value_(other.value_) {}

#include "eckit/io/Offset.b"

    Offset& operator=(const Offset& other) {
        value_ = other.value_;
        return *this;
    }

    bool operator==(const Offset& other) const { return value_ == other.value_; }

    bool operator!=(const Offset& other) const { return value_ != other.value_; }

    bool operator<(const Offset& other) const { return value_ < other.value_; }

    bool operator>(const Offset& other) const { return value_ > other.value_; }

    bool operator<=(const Offset& other) const { return value_ <= other.value_; }

    bool operator>=(const Offset& other) const { return value_ >= other.value_; }

    Offset operator+(const Length& other) const { return Offset(value_ + other.value_); }

    void operator+=(const Length& other) { value_ += other.value_; }

    void operator-=(const Length& other) { value_ -= other.value_; }

    Length operator-(const Offset& other) const { return value_ - other.value_; }


    operator value_t() const { return value_; }

    void dump(DumpLoad&) const;
    void load(DumpLoad&);

private:

    // -- Members

    value_t value_;

    friend class Length;
};

typedef std::vector<Offset> OffsetList;

//-----------------------------------------------------------------------------

// Global routines

// Sort both std::vector according to offset
void sort(OffsetList&, LengthList&);
bool compress(OffsetList&, LengthList&);
void accumulate(const LengthList&, OffsetList&, const Offset& = 0);

#ifdef _AIX
#pragma options align = reset
#endif

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
