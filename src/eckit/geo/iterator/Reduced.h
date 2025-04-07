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

#include "eckit/geo/Iterator.h"


namespace eckit::geo::grid {
class Reduced;
}


namespace eckit::geo::iterator {


class Reduced final : public geo::Iterator {
public:

    // -- Constructors

    explicit Reduced(const Grid&, size_t index = 0);

private:

    // -- Members

    const grid::Reduced& grid_;
    std::vector<double> longitudes_j_;
    const std::vector<double>& latitudes_;
    const std::vector<size_t>& niacc_;
    const size_t size_;
    size_t j_;
    size_t index_;

    // -- Overridden operators

    bool operator==(const Iterator&) const override;
    bool operator++() override;
    bool operator+=(difference_type) override;
    explicit operator bool() const override;
    Point operator*() const override;

    // -- Overridden methods

    size_t index() const override { return index_; }
    size_t j(size_t idx) const;

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::iterator
