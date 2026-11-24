// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit_exceptions.h"

#include "AreaWrapper.h"
#include "eckit-sys/src/geo.rs.h"

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Point.h"

#include <string>

namespace eckit_bridge {

//----------------------------------------------------------------------------------------------------------------------

rust::String AreaWrapper::area_type() const {
    return rust::String(area_->type());
}

rust::String AreaWrapper::uid() const {
    return rust::String(area_->uid());
}

bool AreaWrapper::is_default() const {
    return area_->is_default();
}

bool AreaWrapper::contains(double lon, double lat) const {
    return area_->contains(eckit::geo::PointLonLat{lon, lat});
}

bool AreaWrapper::intersects(const AreaWrapper& bbox) const {
    eckit::geo::area::BoundingBox copy(bbox.inner_bbox());
    return area_->intersects(copy);
}

double AreaWrapper::surface_area() const {
    return area_->area();
}

bool AreaWrapper::equals(const AreaWrapper& other) const {
    return *area_ == *other.area_;
}

// ============== Bounding boxes ==============

bool AreaWrapper::is_bounding_box() const {
    return dynamic_cast<const eckit::geo::area::BoundingBox*>(area_.get()) != nullptr;
}

const eckit::geo::area::BoundingBox& AreaWrapper::inner_bbox() const {
    const auto* bbox = dynamic_cast<const eckit::geo::area::BoundingBox*>(area_.get());
    if (bbox == nullptr) {
        throw eckit::geo::exception::AreaError("Area: not a bounding box (type '" + area_->type() + "')", Here());
    }
    return *bbox;
}

Bbox AreaWrapper::bbox() const {
    const auto& bbox = inner_bbox();
    return {bbox.north(), bbox.west(), bbox.south(), bbox.east()};
}

bool AreaWrapper::bbox_global() const {
    return inner_bbox().global();
}

bool AreaWrapper::bbox_periodic() const {
    return inner_bbox().periodic();
}

bool AreaWrapper::bbox_empty() const {
    return inner_bbox().empty();
}

bool AreaWrapper::bbox_contains(const AreaWrapper& other) const {
    return inner_bbox().contains(other.inner_bbox());
}

// ============== Factories ==============

std::unique_ptr<AreaWrapper> AreaWrapper::from_spec(rust::Str spec) {
    // Adopt the owning pointer before anything else can throw.
    std::unique_ptr<const eckit::geo::Area> area(eckit::geo::AreaFactory::make_from_string(std::string(spec)));
    return std::make_unique<AreaWrapper>(std::move(area));
}

std::unique_ptr<AreaWrapper> AreaWrapper::from_bbox(Bbox bbox) {
    return std::make_unique<AreaWrapper>(
        eckit::geo::area::BoundingBox::make_from_area(bbox.north, bbox.west, bbox.south, bbox.east));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
