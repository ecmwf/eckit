// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/log/SavedStatus.h"

#include "eckit/runtime/Monitor.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

SavedStatus::SavedStatus() : status_(Monitor::instance().status()) {}

SavedStatus::~SavedStatus() {
    Monitor::instance().status(status_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
