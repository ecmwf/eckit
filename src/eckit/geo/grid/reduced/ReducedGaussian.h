// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Projection.h"
#include "eckit/geo/Range.h"
#include "eckit/geo/grid/Reduced.h"
#include "eckit/geo/range/GaussianLatitude.h"
#include "eckit/geo/util.h"


namespace eckit::geo::grid::reduced {


class ReducedGaussian : public Reduced {
public:

    // -- Constructors

    explicit ReducedGaussian(const Spec&);
    explicit ReducedGaussian(const pl_type&, const BoundingBox& = BoundingBox::bounding_box_default(),
                             order::ReducedScan = order::ReducedScan(), std::unique_ptr<const Projection> = nullptr);
    explicit ReducedGaussian(size_t N, const BoundingBox& = BoundingBox::bounding_box_default(),
                             order::ReducedScan = order::ReducedScan(), std::unique_ptr<const Projection> = nullptr);

    // -- Methods

    size_t N() const { return N_; }
    const pl_type& pl() const { return pl_; }

    /// If pl is octahedral over the rows with points, as other rows (e.g. of pl 0) are outside the bounding box
    bool is_reduced_octahedral() const;

    /// If pl is classical (of an N with a known classical pl) over the rows with points, as is_reduced_octahedral
    bool is_reduced_classical() const;

    // -- Overridden methods

    std::string name() const override;

    size_t size() const override { return nxacc().back(); }
    size_t nxj(size_t j) const override { return longitude_.at(j)->size(); }
    const Range& lat() const override { return latitude_; };

    [[nodiscard]] const std::vector<double>& latitudes() const override { return latitude_.values(); }
    [[nodiscard]] const std::vector<double>& longitudes(size_t j) const override { return longitude_.at(j)->values(); }
    double longitude(size_t j, size_t i) const override { return longitude_.at(j)->value(i); }

private:

    // -- Members

    const size_t N_;
    const pl_type pl_;
    const range::GaussianLatitude latitude_;

    std::vector<std::unique_ptr<Range>> longitude_;

    size_t first_ = 0;  ///< first row with points
    size_t last_  = 0;  ///< past the last row with points

    // -- Methods

    /// If pl matches a reference over the rows with points, so the reference and the bounding box make this grid
    bool matches(const pl_type& reference) const;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
    const std::string& type() const override;

    [[nodiscard]] Grid* make_grid_cropped(const Area&) const override;
    [[nodiscard]] Grid* make_grid_reordered(const order_type&) const override;
};


}  // namespace eckit::geo::grid::reduced
