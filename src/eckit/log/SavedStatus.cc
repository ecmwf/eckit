/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/SavedStatus.h"
#include "eckit/runtime/Monitor.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

SavedStatus::SavedStatus() :
    status_(Monitor::instance().status()) {}

SavedStatus::~SavedStatus() {
    Monitor::instance().status(status_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
