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

#include "eckit/geo/order/Scan.h"
#include "eckit/geo/util.h"


namespace eckit::geo::order {


class ReducedScan : public Scan {
public:

    // -- Constructors

    explicit ReducedScan(const pl_type& pl) : pl_(pl) {}
    explicit ReducedScan(const Spec&) {}

    // -- Overridden methods

    const std::string& type() const override;

    const value_type& order_default() const override;
    const value_type& order() const override { return order_; }
    Reordering reorder(const value_type& to) const override;

private:

    // -- Members

    value_type order_;

    const pl_type pl_;
};


}  // namespace eckit::geo::order
