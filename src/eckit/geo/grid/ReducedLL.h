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

#include "eckit/geo/Range.h"
#include "eckit/geo/grid/Reduced.h"
#include "eckit/geo/order/ReducedScan.h"
#include "eckit/geo/util.h"


namespace eckit::geo::grid {


class ReducedLL : public Reduced {
public:

    // -- Constructors

    explicit ReducedLL(const Spec&);
    explicit ReducedLL(const pl_type&, area::BoundingBox* = nullptr);

    // -- Methods

    const pl_type& pl() const { return pl_; }

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    size_t ni(size_t j) const override;
    size_t nj() const override;

    Reordering reorder(order_type) const override;

private:

    // -- Members

    const pl_type pl_;

    std::unique_ptr<Range> x_;
    std::unique_ptr<Range> y_;
    order::ReducedScan order_;

    // -- Overridden methods

    const std::vector<double>& latitudes() const override;
    std::vector<double> longitudes(size_t j) const override;

    void fill_spec(spec::Custom&) const override;
    const std::string& type() const override;
};


}  // namespace eckit::geo::grid
