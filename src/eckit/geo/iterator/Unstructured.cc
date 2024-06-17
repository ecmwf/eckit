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


#include "eckit/geo/iterator/Unstructured.h"

#include <utility>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/grid/Unstructured.h"


namespace eckit::geo::iterator {


namespace {


struct LonLatReference : Unstructured::Container {
    explicit LonLatReference(const std::vector<double>& longitudes, const std::vector<double>& latitudes) :
        longitudes(longitudes), latitudes(latitudes) {
        ASSERT(longitudes.size() == latitudes.size());
    }

    Point get(size_t index) const override { return PointLonLat{longitudes.at(index), latitudes.at(index)}; }
    size_t size() const override { return longitudes.size(); }

    const std::vector<double>& longitudes;
    const std::vector<double>& latitudes;
};


struct PointsReference : Unstructured::Container {
    explicit PointsReference(const std::vector<Point>& points) : points(points) {}

    Point get(size_t index) const override { return points.at(index); }
    size_t size() const override { return points.size(); }

    const std::vector<Point>& points;
};


struct PointsMove : Unstructured::Container {
    explicit PointsMove(std::vector<Point>&& points) : points(points) {}

    Point get(size_t index) const override { return points.at(index); }
    size_t size() const override { return points.size(); }

    const std::vector<Point> points;
};


}  // namespace


Unstructured::Unstructured(const Grid& grid, size_t index, const std::vector<double>& longitudes,
                           const std::vector<double>& latitudes) :
    container_(new LonLatReference(longitudes, latitudes)), index_(index), size_(container_->size()), uid_(grid.uid()) {
    ASSERT(container_->size() == grid.size());
}


Unstructured::Unstructured(const Grid& grid, size_t index, const std::vector<Point>& points) :
    container_(new PointsReference(points)), index_(index), size_(container_->size()), uid_(grid.uid()) {
    ASSERT(container_->size() == grid.size());
}


Unstructured::Unstructured(const Grid& grid, size_t index, std::vector<Point>&& points) :
    container_(new PointsMove(std::move(points))), index_(index), size_(container_->size()), uid_(grid.uid()) {
    ASSERT(container_->size() == grid.size());
}


Unstructured::Unstructured(const Grid& grid) : index_(grid.size()), size_(grid.size()), uid_(grid.uid()) {}


bool Unstructured::operator==(const geo::Iterator& other) const {
    const auto* another = dynamic_cast<const Unstructured*>(&other);
    return another != nullptr && index_ == another->index_ && uid_ == another->uid_;
}


bool Unstructured::operator++() {
    if (index_++; index_ < size_) {
        return true;
    }

    index_ = size_;  // ensure it's invalid
    return false;
}


bool Unstructured::operator+=(difference_type d) {
    if (auto di = static_cast<difference_type>(index_); 0 <= di + d && di + d < static_cast<difference_type>(size_)) {
        index_ = static_cast<size_t>(di + d);
        return true;
    }

    index_ = size_;  // ensure it's invalid
    return false;
}


Unstructured::operator bool() const {
    return index_ < size_;
}


Point Unstructured::operator*() const {
    ASSERT(container_);
    return container_->get(index_);
}


void Unstructured::fill_spec(spec::Custom&) const {
    // FIXME implement
}


}  // namespace eckit::geo::iterator
