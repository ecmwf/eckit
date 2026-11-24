// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// eckit geo Area bridge: wraps `eckit::geo::Area`.
#pragma once

#include "SpecWrapper.h"

#include "eckit/geo/Area.h"
#include "eckit/geo/area/BoundingBox.h"

#include "rust/cxx.h"

#include <memory>

namespace eckit_bridge {

struct Bbox;

//----------------------------------------------------------------------------------------------------------------------

/// Wraps `eckit::geo::Area` for Rust FFI.
///
/// Areas are immutable and built through `eckit::geo::AreaFactory`, which hands
/// back an owning pointer; the wrapper adopts it for its whole lifetime.
class AreaWrapper {
    std::unique_ptr<const eckit::geo::Area> area_;
    SpecWrapper spec_;

public:

    explicit AreaWrapper(std::unique_ptr<const eckit::geo::Area> area) : area_(std::move(area)), spec_(area_->spec()) {}

    rust::String area_type() const;
    rust::String uid() const;

    /// The canonical spec, cached by the area and valid for its lifetime.
    const SpecWrapper& spec() const { return spec_; }

    bool is_default() const;

    /// Whether the (lon, lat) point is in the area.
    bool contains(double lon, double lat) const;

    /// Whether the area intersects a bounding box; an `AreaError` if `bbox` isn't one.
    bool intersects(const AreaWrapper& bbox) const;

    /// Surface area, assuming Earth [L^2].
    double surface_area() const;

    /// Equality by uid, however the areas are described.
    bool equals(const AreaWrapper& other) const;

    // Bounding boxes
    bool is_bounding_box() const;

    /// The box's extent; an `AreaError` if the area isn't a bounding box, as the methods below.
    Bbox bbox() const;
    bool bbox_global() const;
    bool bbox_periodic() const;
    bool bbox_empty() const;
    bool bbox_contains(const AreaWrapper& other) const;

    // Access underlying for other C++ bridge code
    const eckit::geo::Area& inner() const { return *area_; }
    const eckit::geo::area::BoundingBox& inner_bbox() const;

    // ============== Factories ==============

    /// Build an area from a spec, a YAML/JSON mapping, e.g. `{"area":[90,0,-90,360]}`.
    static std::unique_ptr<AreaWrapper> from_spec(rust::Str spec);

    /// Build a bounding box from its extent.
    static std::unique_ptr<AreaWrapper> from_bbox(Bbox bbox);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
