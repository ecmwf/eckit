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


namespace eckit {
class Configuration;
}


namespace eckit::geo {


class Range : protected std::vector<double> {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    Range(const Range&) = delete;
    Range(Range&&)      = delete;

    // -- Destructor

    virtual ~Range() = default;

    // -- Convertors
    // None

    // -- Operators

    Range& operator=(const Range&) = delete;
    Range& operator=(Range&&)      = delete;

    // -- Methods
    // None

    using vector::empty;

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

    virtual const std::vector<double>& valuesVector() const { return *this; }

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


}  // namespace eckit::geo
