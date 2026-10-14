// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Conversions between `eckit::geo::Point` and its FFI form, shared by the bridge wrappers.
#pragma once

#include "eckit/geo/Point.h"

namespace eckit_bridge {

struct RawPoint;

//----------------------------------------------------------------------------------------------------------------------

struct RustPoint {
    /// Flattens a point for FFI. Throws `GridError` on an empty variant.
    static RawPoint to_raw(const eckit::geo::Point&);

    /// Rebuilds the point a flattened one describes.
    static eckit::geo::Point from_raw(const RawPoint&);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
