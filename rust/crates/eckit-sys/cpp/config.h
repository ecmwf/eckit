// eckit Configuration bridge — wraps `eckit::LocalConfiguration`.
#pragma once

#include "eckit/config/LocalConfiguration.h"

#include "rust/cxx.h"

#include <cstddef>
#include <cstdint>
#include <memory>

namespace eckit_bridge {

/// Wraps `eckit::LocalConfiguration` for Rust FFI.
class ConfigWrapper {
    eckit::LocalConfiguration config_;

public:

    ConfigWrapper() = default;
    explicit ConfigWrapper(const eckit::Configuration& other) : config_(other) {}

    // Read
    bool has(rust::Str key) const;
    bool is_list(rust::Str key) const;
    bool is_empty() const;
    rust::String get_string(rust::Str key, rust::Str default_val) const;
    int64_t get_long(rust::Str key, int64_t default_val) const;
    int32_t get_int(rust::Str key, int32_t default_val) const;
    bool get_bool(rust::Str key, bool default_val) const;
    double get_double(rust::Str key, double default_val) const;
    rust::Vec<rust::String> get_string_vector(rust::Str key, const rust::Vec<rust::String>& default_val) const;

    // Sub-configurations (by key)
    std::unique_ptr<ConfigWrapper> get_sub(rust::Str key) const;
    size_t sub_count(rust::Str key) const;
    std::unique_ptr<ConfigWrapper> sub_at(rust::Str key, size_t index) const;

    // Sub-configurations (root-level list)
    size_t root_sub_count() const;
    std::unique_ptr<ConfigWrapper> root_sub_at(size_t index) const;

    // Write
    void set_string(rust::Str key, rust::Str value);
    void set_long(rust::Str key, int64_t value);
    void set_int(rust::Str key, int32_t value);
    void set_bool(rust::Str key, bool value);
    void set_double(rust::Str key, double value);
    void remove(rust::Str key);

    // Access underlying for other C++ bridge code
    const eckit::LocalConfiguration& inner() const { return config_; }
    eckit::LocalConfiguration& inner() { return config_; }
};

// Factory functions — names match cxx bridge declarations
std::unique_ptr<ConfigWrapper> create();
std::unique_ptr<ConfigWrapper> from_path(rust::Str path);
std::unique_ptr<ConfigWrapper> from_yaml(rust::Str yaml);
std::unique_ptr<ConfigWrapper> clone(const ConfigWrapper& src);

}  // namespace eckit_bridge
