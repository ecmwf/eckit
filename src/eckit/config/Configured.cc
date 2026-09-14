// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date Apr 2015


#include "eckit/config/Configured.h"

#include "eckit/exception/Exceptions.h"

namespace eckit {


Configured::Configured() {}


Configured::~Configured() {}

Configured& Configured::set(const std::string& name, long long value) {
    NOTIMP;
    return *this;
}

Configured& Configured::set(const std::string& name, const std::vector<long long>& value) {
    NOTIMP;
    return *this;
}

Configured& Configured::set(const std::string& name, const std::vector<bool>& value) {
    NOTIMP;
    return *this;
}


}  // namespace eckit
