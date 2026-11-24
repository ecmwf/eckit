// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// eckit geo Figure bridge: view of an `eckit::geo::Figure`.
#pragma once

#include "SpecWrapper.h"

#include "eckit/geo/Figure.h"
#include "eckit/spec/Custom.h"

#include "rust/cxx.h"

namespace eckit_bridge {

class AreaWrapper;

//----------------------------------------------------------------------------------------------------------------------

/// View of an `eckit::geo::Figure` for Rust FFI.
///
/// Does not own the figure: a projection hands out the figure it holds, which
/// lives as long as the projection (and the grid holding that) does. The spec
/// is built once, as the figure has none cached.
class FigureWrapper {
    const eckit::geo::Figure& figure_;
    eckit::spec::Custom custom_;
    SpecWrapper spec_;

public:

    explicit FigureWrapper(const eckit::geo::Figure& figure) : figure_(figure), spec_(custom_) {
        figure_.fill_spec(custom_);
    }

    FigureWrapper(const FigureWrapper&) = delete;
    FigureWrapper(FigureWrapper&&)      = delete;

    FigureWrapper& operator=(const FigureWrapper&) = delete;
    FigureWrapper& operator=(FigureWrapper&&)      = delete;

    /// Radius; a `FigureError` for a non-spherical figure.
    double R() const;
    double a() const;
    double b() const;

    /// Surface area [L^2].
    double area() const;

    /// Surface area between the parallels and meridians of a bounding box [L^2]; an `AreaError` if `bbox` isn't one.
    double area_within(const AreaWrapper& bbox) const;

    const SpecWrapper& spec() const { return spec_; }
    rust::String proj_str() const;

    bool spherical() const;
    double eccentricity() const;
    double flattening() const;
    bool is_default() const;

    /// Equality by size, however the figures are described.
    bool equals(const FigureWrapper& other) const;

    // Access underlying for other C++ bridge code
    const eckit::geo::Figure& inner() const { return figure_; }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
