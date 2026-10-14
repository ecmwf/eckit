// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit_exceptions.h"

#include "ProjectionWrapper.h"
#include "RustPoint.h"
#include "RustVec.h"
#include "eckit-sys/src/geo.rs.h"

#include <algorithm>
#include <array>
#include <string>
#include <vector>

namespace eckit_bridge {

namespace {

void copy_blocks(const std::vector<std::vector<double>>& blocks, std::array<rust::Slice<double>, 3> out) {
    if (blocks.size() > out.size()) {
        throw eckit::UserError("ProjectionWrapper: " + std::to_string(blocks.size()) + " output coordinates", Here());
    }
    for (size_t i = 0; i < blocks.size(); ++i) {
        if (out[i].size() < blocks[i].size()) {
            throw eckit::UserError("ProjectionWrapper: output buffer " + std::to_string(i + 1) + " too small for " +
                                       std::to_string(blocks[i].size()) + " points",
                                   Here());
        }
        std::copy(blocks[i].begin(), blocks[i].end(), out[i].begin());
    }
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
    return RustVec::strings(projection_.source_point_coordinates());
}

rust::Vec<rust::String> ProjectionWrapper::target_point_coordinates() const {
    return RustVec::strings(projection_.target_point_coordinates());
}

size_t ProjectionWrapper::source_dims() const {
    return projection_.source_point_coordinates().size();
}

size_t ProjectionWrapper::target_dims() const {
    return projection_.target_point_coordinates().size();
}

RawPoint ProjectionWrapper::fwd(RawPoint point) const {
    return RustPoint::to_raw(projection_.fwd(RustPoint::from_raw(point)));
}

RawPoint ProjectionWrapper::inv(RawPoint point) const {
    return RustPoint::to_raw(projection_.inv(RustPoint::from_raw(point)));
}

void ProjectionWrapper::fwd_vectors(rust::Slice<const double> v1, rust::Slice<const double> v2,
                                    rust::Slice<const double> v3, rust::Slice<double> out1, rust::Slice<double> out2,
                                    rust::Slice<double> out3) const {
    copy_blocks(projection_.fwd(RustVec::to_std(v1), RustVec::to_std(v2), RustVec::to_std(v3)), {out1, out2, out3});
}

void ProjectionWrapper::inv_vectors(rust::Slice<const double> v1, rust::Slice<const double> v2,
                                    rust::Slice<const double> v3, rust::Slice<double> out1, rust::Slice<double> out2,
                                    rust::Slice<double> out3) const {
    copy_blocks(projection_.inv(RustVec::to_std(v1), RustVec::to_std(v2), RustVec::to_std(v3)), {out1, out2, out3});
}

bool ProjectionWrapper::equals(const ProjectionWrapper& other) const {
    return projection_ == other.projection_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
