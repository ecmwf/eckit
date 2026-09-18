// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Dec 2018

#include "eckit/config/Parametrisation.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {

Parametrisation::~Parametrisation() {}

bool Parametrisation::get(const std::string& name, long long& value) const {
    NOTIMP;
}

bool Parametrisation::get(const std::string& name, std::vector<long long>& value) const {
    NOTIMP;
}

bool Parametrisation::get(const std::string& name, std::vector<bool>& value) const {
    NOTIMP;
}

}  // namespace eckit
