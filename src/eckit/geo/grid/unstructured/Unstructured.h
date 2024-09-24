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

#include "eckit/geo/grid/Unstructured.h"


namespace eckit::geo::grid::unstructured {


class Unstructured final : public grid::Unstructured {
public:
    // -- Constructors

    explicit Unstructured(const std::vector<Point> &);

    // -- Class methods

    [[nodiscard]] static Unstructured* make_from_latlon(const std::vector<double> &, const std::vector<double> &);
};


}  // namespace eckit::geo::grid::unstructured
