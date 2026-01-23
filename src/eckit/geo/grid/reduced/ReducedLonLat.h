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

#include <memory>
#include <vector>

#include "eckit/geo/grid/Reduced.h"
#include "eckit/geo/order/Scan.h"
#include "eckit/geo/range/Regular.h"
#include "eckit/geo/util.h"


namespace eckit::geo::grid::reduced {


class ReducedLonLat : public Reduced {
public:

    // -- Constructors

    explicit ReducedLonLat(const Spec&);
    explicit ReducedLonLat(const pl_type&, const BoundingBox& = BoundingBox::bounding_box_default());

    // -- Methods

    const pl_type& pl() const { return pl_; }

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    size_t size() const override;
    size_t nx(size_t j) const override;
    size_t ny() const override;

    const order_type& order() const override { return scan_.order(); }
    renumber_type reorder(const order_type& to) const override { return scan_.reorder(to, pl_); }

    const std::vector<double>& latitudes() const override;
    std::vector<double> longitudes(size_t j) const override;

private:

    // -- Members

    const pl_type pl_;

    mutable std::vector<std::unique_ptr<range::RegularLongitude>> longitude_;
    std::unique_ptr<range::RegularLatitude> latitude_;
    order::Scan scan_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
    const std::string& type() const override;
};


}  // namespace eckit::geo::grid::reduced
