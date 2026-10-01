// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Conversions into `rust::Vec`, shared by the bridge wrappers.
#pragma once

#include "rust/cxx.h"

#include <vector>

namespace eckit_bridge {

//----------------------------------------------------------------------------------------------------------------------

/// Builds a `rust::Vec` from a C++ container. Always a copy: `rust::Vec` owns
/// Rust-allocated storage, so it cannot adopt a `std::vector` buffer.
struct RustVec {
    /// Copies `values` into a `rust::Vec<To>`, converting each element.
    template <typename To, typename From>
    static rust::Vec<To> copy(const std::vector<From>& values) {
        rust::Vec<To> out;
        out.reserve(values.size());
        for (const auto& v : values) {
            out.push_back(static_cast<To>(v));
        }
        return out;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
