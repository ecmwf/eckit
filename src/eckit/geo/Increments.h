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


#pragma once

#include <array>
#include <ostream>


namespace eckit {
class Configuration;
}


namespace eckit::geo {


class Increments : protected std::array<double, 2> {
public:
    // -- Exceptions
    // None

    // -- Constructors

    explicit Increments(const Configuration&);

    Increments(double west_east, double south_north);
    Increments() :
        Increments(0, 0) {}

    Increments(const Increments& other) :
        array(other) {}

    Increments(Increments&&) = default;

    // -- Destructor

    ~Increments() = default;

    // -- Convertors
    // None

    // -- Operators

    bool operator==(const Increments& other) const;
    bool operator!=(const Increments& other) const { return !operator==(other); }

    Increments& operator=(const Increments& other) {
        array::operator=(other);
        return *this;
    }

    Increments& operator=(Increments&& other) {
        array::operator=(other);
        return *this;
    }

    // Members

    double& west_east   = array::operator[](0);
    double& south_north = array::operator[](1);

    // -- Methods

    std::array<double, 2> deconstruct() const { return {west_east, south_north}; }

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    friend std::ostream& operator<<(std::ostream& os, const Increments& inc) {
        return os << "[" << inc.west_east << "," << inc.south_north << "]";
    }
};


}  // namespace eckit::geo
