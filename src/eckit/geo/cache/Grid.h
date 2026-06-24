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

#include "eckit/geo/Grid.h"


namespace eckit::geo::cache {


class Grid final {
public:

    // -- Class methods

    static Grid& instance();

    static void save(const geo::Grid::uid_type&, const geo::Grid::Spec&);

private:

    // -- Constructors

    explicit Grid() = default;
};


}  // namespace eckit::geo::cache
