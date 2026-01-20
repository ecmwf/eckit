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

    // -- Methods

    static LibEcKitGeo& instance();

    static std::vector<PathName> shareArea();
    static std::vector<PathName> shareGrid();
    static std::vector<PathName> shareProjection();

    static bool caching();
    static std::string cacheDir();

    static bool proj();

private:

    // -- Constructors

    LibEcKitGeo();

    // -- Overridden methods

    [[nodiscard]] const void* addr() const override;
    std::string version() const override;
    std::string gitsha1(unsigned int count) const override;
};


}  // namespace eckit
