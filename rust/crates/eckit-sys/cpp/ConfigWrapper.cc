// eckit Configuration bridge — implementation.

#include "eckit_exceptions.h"

#include "ConfigWrapper.h"
#include "eckit-sys/src/lib.rs.h"

#include "eckit/config/YAMLConfiguration.h"
#include "eckit/filesystem/PathName.h"

namespace eckit_bridge {

//----------------------------------------------------------------------------------------------------------------------

bool ConfigWrapper::has(rust::Str key) const {
    return config_.has(std::string(key));
}

bool ConfigWrapper::is_list(rust::Str key) const {
    return config_.isList(std::string(key));
}

bool ConfigWrapper::is_empty() const {
    return config_.empty();
}

rust::String ConfigWrapper::get_string(rust::Str key, rust::Str default_val) const {
    return rust::String(config_.getString(std::string(key), std::string(default_val)));
}

int64_t ConfigWrapper::get_long(rust::Str key, int64_t default_val) const {
    return config_.getLong(std::string(key), default_val);
}

int32_t ConfigWrapper::get_int(rust::Str key, int32_t default_val) const {
    return config_.getInt(std::string(key), default_val);
}

bool ConfigWrapper::get_bool(rust::Str key, bool default_val) const {
    return config_.getBool(std::string(key), default_val);
}

double ConfigWrapper::get_double(rust::Str key, double default_val) const {
    return config_.getDouble(std::string(key), default_val);
}

rust::Vec<rust::String> ConfigWrapper::get_string_vector(rust::Str key,
                                                         const rust::Vec<rust::String>& default_val) const {
    std::vector<std::string> def;
    def.reserve(default_val.size());
    for (const auto& s : default_val) {
        def.emplace_back(std::string(s));
    }
    auto vec = config_.getStringVector(std::string(key), def);
    rust::Vec<rust::String> result;
    result.reserve(vec.size());
    for (const auto& s : vec) {
        result.push_back(rust::String(s));
    }
    return result;
}

std::unique_ptr<ConfigWrapper> ConfigWrapper::get_sub(rust::Str key) const {
    return std::make_unique<ConfigWrapper>(config_.getSubConfiguration(std::string(key)));
}

size_t ConfigWrapper::sub_count(rust::Str key) const {
    return config_.getSubConfigurations(std::string(key)).size();
}

std::unique_ptr<ConfigWrapper> ConfigWrapper::sub_at(rust::Str key, size_t index) const {
    auto subs = config_.getSubConfigurations(std::string(key));
    return std::make_unique<ConfigWrapper>(subs.at(index));
}

size_t ConfigWrapper::root_sub_count() const {
    return config_.getSubConfigurations().size();
}

std::unique_ptr<ConfigWrapper> ConfigWrapper::root_sub_at(size_t index) const {
    auto subs = config_.getSubConfigurations();
    return std::make_unique<ConfigWrapper>(subs.at(index));
}

void ConfigWrapper::set_string(rust::Str key, rust::Str value) {
    config_.set(std::string(key), std::string(value));
}

void ConfigWrapper::set_long(rust::Str key, int64_t value) {
    config_.set(std::string(key), static_cast<long>(value));
}

void ConfigWrapper::set_int(rust::Str key, int32_t value) {
    config_.set(std::string(key), value);
}

void ConfigWrapper::set_bool(rust::Str key, bool value) {
    config_.set(std::string(key), value);
}

void ConfigWrapper::set_double(rust::Str key, double value) {
    config_.set(std::string(key), value);
}

void ConfigWrapper::remove(rust::Str key) {
    config_.remove(std::string(key));
}

//----------------------------------------------------------------------------------------------------------------------

std::unique_ptr<ConfigWrapper> create() {
    return std::make_unique<ConfigWrapper>();
}

std::unique_ptr<ConfigWrapper> from_path(rust::Str path) {
    auto p    = eckit::PathName{std::string(path)};
    auto yaml = eckit::YAMLConfiguration{p};
    return std::make_unique<ConfigWrapper>(yaml);
}

std::unique_ptr<ConfigWrapper> from_yaml(rust::Str yaml) {
    auto str    = std::string(yaml);
    auto parsed = eckit::YAMLConfiguration{str};
    return std::make_unique<ConfigWrapper>(parsed);
}

std::unique_ptr<ConfigWrapper> clone(const ConfigWrapper& src) {
    return std::make_unique<ConfigWrapper>(src.inner());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
