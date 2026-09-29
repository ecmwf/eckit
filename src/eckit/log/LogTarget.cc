// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/exception/Exceptions.h"

#include "eckit/log/LogTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

LogTarget::LogTarget() {}

LogTarget::~LogTarget() {}

void LogTarget::print(std::ostream& s) const {
    NOTIMP;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
