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
#include <vector>


namespace grit::pl {


class Pl {
public:
    // -- Types

    using pl_value_type = long;
    using pl_type       = std::vector<pl_value_type>;

    // -- Exceptions
    // None

    // -- Constructors

    Pl()          = default;
    Pl(const Pl&) = delete;
    Pl(Pl&&)      = delete;

    // -- Destructor

    virtual ~Pl() = default;

    // -- Convertors
    // None

    // -- Operators

    Pl& operator=(const Pl&) = delete;
    Pl& operator=(Pl&&)      = delete;

    // -- Methods
    // None

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

    virtual const pl_type& get_pl(size_t N) const      = 0;
    virtual const pl_type& get_pl_half(size_t N) const = 0;

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


}  // namespace grit::pl
