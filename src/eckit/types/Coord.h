/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// Claude  Gibert - ECMWF Dec 97

#ifndef eckit_Coord_h
#define eckit_Coord_h

#include <iosfwd>
#include <vector>

#include "eckit/serialisation/Stream.h"


namespace eckit {

//-----------------------------------------------------------------------------

class DumpLoad;

class Coord {
public:

    friend std::ostream& operator<<(std::ostream& s, const Coord& x) {
        x.print(s);
        return s;
    }

    friend Stream& operator>>(Stream& s, Coord& x) { return s >> x.value_; }

    // -- Contructors

    Coord(double l = 0) : value_(l) {}
    Coord(const Coord& other) : value_(other.value_) {}


    // -- Operators

    Coord& operator=(const Coord& other) {
        value_ = other.value_;
        return *this;
    }

    Coord operator+(const Coord& other) const { return Coord(value_ + other.value_); }

    Coord& operator+=(const Coord& other) {
        value_ += other.value_;
        return *this;
    }

    bool operator==(const Coord& other) const { return value_ == other.value_; }

    bool operator<(const Coord& other) const { return value_ < other.value_; }

    bool operator<=(const Coord& other) const { return value_ <= other.value_; }

    bool operator>(const Coord& other) const { return value_ > other.value_; }

    bool operator>=(const Coord& other) const { return value_ >= other.value_; }

// --
#if 0
	double operator-(const Coord& other) const
		{ return value_ - other.value_;}
#endif

    // -- Methods

    void print(std::ostream& s) const;

    void dump(DumpLoad&) const;
    void load(DumpLoad&);

    // -- Class methods

    double value() { return value_; }

protected:

    // -- Members

    double value_;


    friend class Area;
};

typedef std::vector<Coord> CoordList;

//---------------------------------------------------------
// Longitude
//---------------------------------------------------------

class Longitude : public Coord {
public:

    // -- Contructors

    Longitude(double l = 0);
};

//---------------------------------------------------------
// Latitude
//---------------------------------------------------------

class Latitude : public Coord {
public:

    // -- Contructors

    Latitude(double l = 0);
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
