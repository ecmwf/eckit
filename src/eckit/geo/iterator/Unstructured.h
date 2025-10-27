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

#include "eckit/geo/Iterator.h"


namespace eckit::geo {
class Projection;
namespace container {
class PointsContainer;
}
}  // namespace eckit::geo


namespace eckit::geo::iterator {


class Unstructured : public Iterator {
public:

    // -- Constructors

    Unstructured(const Grid&, size_t index, std::shared_ptr<container::PointsContainer>);
    explicit Unstructured(const Grid&);

private:

    // -- Members

    const Projection& projection_;

    std::shared_ptr<container::PointsContainer> container_;
    size_t index_;
    const size_t size_;
    const std::string uid_;

    // -- Overridden methods

    bool operator==(const geo::Iterator&) const override;
    bool operator++() override;
    bool operator+=(difference_type) override;

    explicit operator bool() const override;
    Point operator*() const override;

    size_t index() const override { return index_; }
};


}  // namespace eckit::geo::iterator
