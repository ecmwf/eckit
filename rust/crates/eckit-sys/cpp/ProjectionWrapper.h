// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// eckit geo Projection bridge: view of an `eckit::geo::Projection`.
#pragma once

#include "FigureWrapper.h"
#include "SpecWrapper.h"

#include "eckit/geo/Projection.h"

#include "rust/cxx.h"

#include <cstddef>

namespace eckit_bridge {

struct RawPoint;

//----------------------------------------------------------------------------------------------------------------------

/// View of an `eckit::geo::Projection` for Rust FFI.
///
/// Does not own the projection: a grid hands out the projection it holds,
/// which lives as long as the grid does.
class ProjectionWrapper {
    const eckit::geo::Projection& projection_;
    FigureWrapper figure_;
    SpecWrapper spec_;

public:

    explicit ProjectionWrapper(const eckit::geo::Projection& projection) :
        projection_(projection), figure_(projection.figure()), spec_(projection.spec()) {}

    ProjectionWrapper(const ProjectionWrapper&) = delete;
    ProjectionWrapper(ProjectionWrapper&&)      = delete;

    ProjectionWrapper& operator=(const ProjectionWrapper&) = delete;
    ProjectionWrapper& operator=(ProjectionWrapper&&)      = delete;

    rust::String projection_type() const;
    const SpecWrapper& spec() const { return spec_; }
    rust::String proj_str() const;

    bool is_default() const;
    bool reverse() const;

    rust::Vec<rust::String> source_point_coordinates() const;
    rust::Vec<rust::String> target_point_coordinates() const;

    /// Number of coordinates of the source (target) points, 2 or 3.
    size_t source_dims() const;
    size_t target_dims() const;

    const FigureWrapper& figure() const { return figure_; }

    /// Project a point, from the source to the target point type (fwd) or the reverse (inv).
    RawPoint fwd(RawPoint point) const;
    RawPoint inv(RawPoint point) const;

    /// Project points given one slice per source coordinate (`v3` empty for points of 2 coordinates), writing one
    /// slice per target coordinate (`out3` unused for targets of 2 coordinates). Points failing to project are NaN.
    void fwd_vectors(rust::Slice<const double> v1, rust::Slice<const double> v2, rust::Slice<const double> v3,
                     rust::Slice<double> out1, rust::Slice<double> out2, rust::Slice<double> out3) const;
    void inv_vectors(rust::Slice<const double> v1, rust::Slice<const double> v2, rust::Slice<const double> v3,
                     rust::Slice<double> out1, rust::Slice<double> out2, rust::Slice<double> out3) const;

    /// Equality by spec, however the projections are described.
    bool equals(const ProjectionWrapper& other) const;

    // Access underlying for other C++ bridge code
    const eckit::geo::Projection& inner() const { return projection_; }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
