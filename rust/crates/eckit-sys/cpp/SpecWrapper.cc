// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// eckit spec bridge — implementation.

#include "eckit_exceptions.h"

#include "RustVec.h"
#include "SpecWrapper.h"

#include <string>

namespace eckit_bridge {

//----------------------------------------------------------------------------------------------------------------------

bool SpecWrapper::has(rust::Str key) const {
    return spec_.has(std::string(key));
}

rust::String SpecWrapper::get_string(rust::Str key) const {
    return rust::String(spec_.get_string(std::string(key)));
}

bool SpecWrapper::get_bool(rust::Str key) const {
    return spec_.get_bool(std::string(key));
}

std::int64_t SpecWrapper::get_long(rust::Str key) const {
    return spec_.get_long(std::string(key));
}

size_t SpecWrapper::get_unsigned(rust::Str key) const {
    return spec_.get_unsigned(std::string(key));
}

double SpecWrapper::get_double(rust::Str key) const {
    return spec_.get_double(std::string(key));
}

rust::Vec<std::int64_t> SpecWrapper::get_long_vector(rust::Str key) const {
    return RustVec::copy<std::int64_t>(spec_.get_long_vector(std::string(key)));
}

rust::Vec<size_t> SpecWrapper::get_unsigned_vector(rust::Str key) const {
    return RustVec::copy<size_t>(spec_.get_unsigned_vector(std::string(key)));
}

rust::Vec<double> SpecWrapper::get_double_vector(rust::Str key) const {
    return RustVec::copy<double>(spec_.get_double_vector(std::string(key)));
}

rust::String SpecWrapper::json() const {
    return rust::String(spec_.str());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
