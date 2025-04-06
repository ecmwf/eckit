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


namespace eckit::geo::order {


class RegularScan : public Scan {
public:

    // -- Constructors

    explicit RegularScan(size_t nx, size_t ny) : nx_(nx), ny_(ny) {}
    explicit RegularScan(const Spec&) {}

    // -- Overridden methods

    const std::string& type() const override;

    Reordering reorder(value_type from, value_type to) const override;

    void fill_spec(spec::Custom&) const override {}

private:

    // -- Members

    const size_t nx_ = 0;
    const size_t ny_ = 0;
};


}  // namespace eckit::geo::order
