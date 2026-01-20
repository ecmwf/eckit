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


namespace eckit::geo {
class Spec;
}


namespace eckit::geo {


class Increments : public std::array<double, 2> {
public:

    // -- Types

    using container_type = array;

    // -- Constructors

    explicit Increments(const Spec& spec) : Increments(make_from_spec(spec)) {}

    Increments(value_type dx, value_type dy);

    Increments(const Increments& other) : container_type(other) {}

    Increments(Increments&& other) : container_type(other) {}

    // -- Destructor

    ~Increments() = default;

    // -- Operators

    bool operator==(const Increments& other) const;
    bool operator!=(const Increments& other) const { return !operator==(other); }

    Increments& operator=(const Increments& other) {
        container_type::operator=(other);
        return *this;
    }

    Increments& operator=(Increments&& other) {
        container_type::operator=(other);
        return *this;
    }

    // Members

    const value_type& dx = container_type::operator[](0);
    const value_type& dy = container_type::operator[](1);

    // -- Methods

    container_type deconstruct() const { return {dx, dy}; }

    // -- Class methods

    static Increments make_from_spec(const Spec&);

private:

    // -- Friends

    friend std::ostream& operator<<(std::ostream& os, const Increments& inc) {
        return os << "[" << inc.dx << "," << inc.dy << "]";
    }
};


}  // namespace eckit::geo
