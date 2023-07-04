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


namespace eckit::geometry {


class Scanner {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    Scanner(const Scanner&) = delete;
    Scanner(Scanner&&)      = delete;

    // -- Destructor

    virtual ~Scanner() = default;

    // -- Convertors
    // None

    // -- Operators

    Scanner& operator=(const Scanner&) = delete;
    Scanner& operator=(Scanner&&)      = delete;

    virtual bool operator++() = 0;

    // -- Methods

    virtual size_t size() const = 0;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Constructors

    Scanner() = default;

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
    // None
};


}  // namespace eckit::geometry
