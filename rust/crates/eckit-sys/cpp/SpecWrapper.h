// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// eckit spec bridge — read-only view of an `eckit::spec::Spec`.
#pragma once

#include "eckit/spec/Spec.h"

#include "rust/cxx.h"

#include <cstddef>
#include <cstdint>

namespace eckit_bridge {

//----------------------------------------------------------------------------------------------------------------------

/// Read-only view of an `eckit::spec::Spec` for Rust FFI.
///
/// Does not own the spec: a `GridWrapper` hands out the spec its grid caches,
/// which lives as long as the grid does.
class SpecWrapper {
    const eckit::spec::Spec& spec_;

public:

    explicit SpecWrapper(const eckit::spec::Spec& spec) : spec_(spec) {}

    bool has(rust::Str key) const;

    // Typed access; a missing key, or one holding another type, throws `SpecError`.
    rust::String get_string(rust::Str key) const;
    bool get_bool(rust::Str key) const;
    std::int64_t get_long(rust::Str key) const;
    size_t get_unsigned(rust::Str key) const;
    double get_double(rust::Str key) const;
    rust::Vec<std::int64_t> get_long_vector(rust::Str key) const;
    rust::Vec<size_t> get_unsigned_vector(rust::Str key) const;
    rust::Vec<double> get_double_vector(rust::Str key) const;

    /// Canonical JSON form.
    rust::String json() const;

    // Access underlying for other C++ bridge code
    const eckit::spec::Spec& inner() const { return spec_; }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
