// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// Conversions between `rust::Vec`/`rust::Slice` and `std::vector`, shared by the bridge wrappers.
#pragma once

#include "rust/cxx.h"

#include <string>
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

    /// Copies strings into a `rust::Vec<rust::String>`.
    static rust::Vec<rust::String> strings(const std::vector<std::string>& values) {
        rust::Vec<rust::String> out;
        out.reserve(values.size());
        for (const auto& v : values) {
            out.push_back(rust::String(v));
        }
        return out;
    }

    /// Copies a slice into a `std::vector`.
    static std::vector<double> to_std(rust::Slice<const double> slice) { return {slice.begin(), slice.end()}; }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
