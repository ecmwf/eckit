// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Projection.h"
#include "eckit/geo/grid/Reduced.h"
#include "eckit/geo/range/Regular.h"
#include "eckit/geo/util.h"


namespace eckit::geo::grid::reduced {


class ReducedLonLat : public Reduced {
public:

    // -- Constructors

    explicit ReducedLonLat(const Spec&);
    explicit ReducedLonLat(const pl_type&, const BoundingBox& = BoundingBox::bounding_box_default(),
                           order::ReducedScan = order::ReducedScan(), std::unique_ptr<const Projection> = nullptr);

    // -- Methods

    const pl_type& pl() const { return pl_; }

    // -- Overridden methods

    size_t size() const override;
    size_t nxj(size_t j) const override;
    const Range& lat() const override { return *latitude_; }


    [[nodiscard]] const std::vector<double>& latitudes() const override;
    [[nodiscard]] const std::vector<double>& longitudes(size_t j) const override;
    double longitude(size_t j, size_t i) const override { return longitude_.at(j)->value(i); }

private:

    // -- Members

    const pl_type pl_;

    std::vector<std::unique_ptr<range::RegularLongitude>> longitude_;
    std::unique_ptr<range::RegularLatitude> latitude_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
    const std::string& type() const override;

    [[nodiscard]] Grid* make_grid_reordered(const order_type&) const override;
};


}  // namespace eckit::geo::grid::reduced
