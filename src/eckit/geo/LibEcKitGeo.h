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

#include <vector>

#include "eckit/system/Library.h"


namespace eckit {
class PathName;
}


namespace eckit {


class LibEcKitGeo final : public system::Library {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors
    // None

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    static LibEcKitGeo& instance();

    static std::vector<PathName> etcGrid();

    static bool caching();
    static std::string cacheDir();

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Constructors

    LibEcKitGeo();

    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods

    const void* addr() const override;
    std::string version() const override;
    std::string gitsha1(unsigned int count) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit
