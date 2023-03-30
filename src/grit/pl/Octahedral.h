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

#include "grit/pl/Pl.h"


namespace grit::pl {


class Octahedral final : public Pl {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    static const pl_type& pl(size_t N);

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Constructors

    Octahedral() = default;

    // -- Destructor
    // None

    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods

    const pl_type& get_pl(size_t N) const override;
    const pl_type& get_pl_half(size_t N) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace grit::pl
