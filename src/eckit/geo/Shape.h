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


class Shape final : public std::array<size_t, 2> {
public:

    // -- Types

    using container_type = array;

    // -- Constructors

    explicit Shape(const Spec& spec) : Shape(make_from_spec(spec)) {}

    Shape(value_type nx, value_type ny);

    Shape() : Shape(0, 0) {}

    Shape(const Shape& other) : container_type(other) {}

    Shape(Shape&& other) : container_type(other) {}

    // -- Destructor

    ~Shape() = default;

    // -- Operators

    bool operator==(const Shape& other) const { return nx == other.nx && ny == other.ny; }

    bool operator!=(const Shape& other) const { return !operator==(other); }

    Shape& operator=(const Shape& other) {
        container_type::operator=(other);
        return *this;
    }

    Shape& operator=(Shape&& other) {
        container_type::operator=(other);
        return *this;
    }

    // Members

    const value_type& nx = container_type::operator[](0);
    const value_type& ny = container_type::operator[](1);

    // -- Methods

    container_type deconstruct() const { return {nx, ny}; }

    // -- Class methods

    static Shape make_from_spec(const Spec&);

private:

    // -- Friends

    friend std::ostream& operator<<(std::ostream& os, const Shape& inc) {
        return os << "[" << inc.nx << "," << inc.ny << "]";
    }
};


}  // namespace eckit::geo
