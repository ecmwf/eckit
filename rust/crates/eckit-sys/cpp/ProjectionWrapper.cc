// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit_exceptions.h"

#include "ProjectionWrapper.h"
#include "RustPoint.h"
#include "eckit-sys/src/geo.rs.h"

#include <vector>

namespace eckit_bridge {

namespace {

std::vector<double> to_vector(rust::Slice<const double> slice) {
    return {slice.begin(), slice.end()};
}

rust::Vec<double> concatenate(const std::vector<std::vector<double>>& blocks) {
    size_t n = 0;
    for (const auto& block : blocks) {
        n += block.size();
    }

    rust::Vec<double> out;
    out.reserve(n);
    for (const auto& block : blocks) {
        for (auto v : block) {
            out.push_back(v);
        }
    }
    return out;
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

rust::String ProjectionWrapper::projection_type() const {
    return rust::String(projection_.type());
}

rust::String ProjectionWrapper::proj_str() const {
    return rust::String(projection_.proj_str());
}

bool ProjectionWrapper::is_default() const {
    return projection_.is_default();
}

bool ProjectionWrapper::reverse() const {
    return projection_.reverse();
}

rust::Vec<rust::String> ProjectionWrapper::source_point_coordinates() const {
    return RustPoint::names(projection_.source_point_coordinates());
}

rust::Vec<rust::String> ProjectionWrapper::target_point_coordinates() const {
    return RustPoint::names(projection_.target_point_coordinates());
}

RawPoint ProjectionWrapper::fwd(RawPoint point) const {
    return RustPoint::to_raw(projection_.fwd(RustPoint::from_raw(point)));
}

RawPoint ProjectionWrapper::inv(RawPoint point) const {
    return RustPoint::to_raw(projection_.inv(RustPoint::from_raw(point)));
}

rust::Vec<double> ProjectionWrapper::fwd_vectors(rust::Slice<const double> v1, rust::Slice<const double> v2,
                                                 rust::Slice<const double> v3) const {
    return concatenate(projection_.fwd(to_vector(v1), to_vector(v2), to_vector(v3)));
}

rust::Vec<double> ProjectionWrapper::inv_vectors(rust::Slice<const double> v1, rust::Slice<const double> v2,
                                                 rust::Slice<const double> v3) const {
    return concatenate(projection_.inv(to_vector(v1), to_vector(v2), to_vector(v3)));
}

bool ProjectionWrapper::equals(const ProjectionWrapper& other) const {
    return projection_ == other.projection_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
