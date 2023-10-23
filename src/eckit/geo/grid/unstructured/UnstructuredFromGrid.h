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

#include <array>
#include <cstddef>
#include <cstdint>

#include "eckit/geo/grid/Unstructured.h"


namespace eckit {
class PathName;
}


namespace eckit::geo::grid::unstructured {


class UnstructuredFromGrid final : public Unstructured {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit UnstructuredFromGrid(const std::vector<Point>&);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    const area::BoundingBox& boundingBox() const override;

    size_t size() const override { return latitudes_.size(); }
    uid_t uid() const override;

    bool includesNorthPole() const override { return true; }
    bool includesSouthPole() const override { return true; }
    bool isPeriodicWestEast() const override { return true; }

    std::vector<Point> to_points() const override;
    std::pair<std::vector<double>, std::vector<double>> to_latlon() const override { return {latitudes_, longitudes_}; }

    const std::vector<double>& longitudes() const override { return longitudes_; }
    const std::vector<double>& latitudes() const override { return latitudes_; }

    // -- Class members
    // None

    // -- Class methods

    static Configuration* config(const std::string& name);

private:
    // -- Members

    mutable std::vector<double> latitudes_;
    mutable std::vector<double> longitudes_;

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


}  // namespace eckit::geo::grid::unstructured
