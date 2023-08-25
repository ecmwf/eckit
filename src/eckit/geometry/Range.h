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

#include <string>
#include <vector>

#include "eckit/memory/Builder.h"
#include "eckit/memory/Factory.h"


namespace eckit {
class Configuration;
}


namespace eckit::geometry {


class Range : protected std::vector<double> {
public:
    // -- Types

    using builder_t = BuilderT1<Range>;
    using ARG1      = const Configuration&;

    // -- Exceptions
    // None

    // -- Constructors

    Range(const Range&) = default;
    Range(Range&&)      = default;

    // -- Destructor

    virtual ~Range() = default;

    // -- Convertors
    // None

    // -- Operators

    Range& operator=(const Range&) = default;
    Range& operator=(Range&&)      = default;

    // -- Methods

    static std::string className() { return "range"; }

    bool empty() const { return vector::empty(); }
    size_t size() const { return empty() ? n_ : vector::size(); }

    virtual const std::vector<double>& values() const = 0;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Constructors

    explicit Range(const Configuration&);
    explicit Range(size_t n);

    // -- Members
    // None

    // -- Methods

    virtual const std::vector<double>& valuesVector() const {
        return *this;
    }

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    const size_t n_;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


// using RangeFactory = Factory<Range>;

// template <typename T>
// using RangeBuilder = ConcreteBuilderT1<Range, T>;


}  // namespace eckit::geometry
