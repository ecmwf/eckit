// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <array>
#include <ostream>


namespace eckit::spec {
class Spec;
}


namespace eckit::geo {


class Shape final : public std::array<size_t, 2> {
public:

    // -- Types

    using container_type = array;

    // -- Constructors

    explicit Shape(const spec::Spec& spec) : Shape(make_from_spec(spec)) {}

    Shape(value_type nx, value_type ny);

    Shape() : Shape(0, 0) {}

    Shape(const Shape& other) : container_type(other) {}

    Shape(Shape&& other) : container_type(other) {}

    // -- Destructor

    ~Shape() = default;

    // -- Operators

    bool operator==(const Shape& other) const { return nx() == other.nx() && ny() == other.ny(); }

    bool operator!=(const Shape& other) const { return !operator==(other); }

    Shape& operator=(const Shape& other) {
        container_type::operator=(other);
        return *this;
    }

    Shape& operator=(Shape&& other) {
        container_type::operator=(other);
        return *this;
    }

    // -- Methods

    value_type nx() const { return container_type::operator[](0); }
    value_type ny() const { return container_type::operator[](1); }

    container_type deconstruct() const { return {nx(), ny()}; }

    // -- Class methods

    static Shape make_from_spec(const spec::Spec&);

private:

    // -- Friends

    friend std::ostream& operator<<(std::ostream& os, const Shape& inc) {
        return os << "[" << inc.nx() << "," << inc.ny() << "]";
    }
};


}  // namespace eckit::geo
