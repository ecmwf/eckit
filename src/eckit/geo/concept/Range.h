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

#include <cstddef>


namespace eckit::geo::concept {


    class Range {
    public:
        // -- Types
        // None

        // -- Exceptions
        // None

        // -- Constructors

        Range(double a, double b, double inc, double ref);

        Range(double a, double b, double inc);

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

        size_t n() const { return n_; }
        double a() const { return a_; }
        double b() const { return b_; }

        // -- Overridden methods
        // None

        // -- Class members
        // None

        // -- Class methods
        // None

    protected:
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

    private:
        // -- Members

        size_t n_;
        double a_;
        double b_;

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


}  // namespace eckit::geo::concept
