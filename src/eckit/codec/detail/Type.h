// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

namespace eckit::codec {

struct Type {
    const std::string name_;
    explicit operator const std::string&() { return name_; }
    operator bool() const { return !name_.empty(); }
    Type(const char* name) : name_(name) {}
    explicit Type(const std::string& name) : name_(name) {}
    bool operator==(const Type& other) const { return name_ == other.name_; }
};


}  // namespace eckit::codec
