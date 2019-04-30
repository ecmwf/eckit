/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/log/Progress.h"
#include "eckit/runtime/Monitor.h"


//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Progress::Progress(const std::string& name, unsigned long long min, unsigned long long max) {
    Monitor::instance().progress(name, min, max);
}

Progress::~Progress() {
    Monitor::instance().progress();
}

void Progress::operator()(unsigned long long value) {
    Monitor::instance().progress(value);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
