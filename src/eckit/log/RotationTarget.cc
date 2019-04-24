/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <fstream>

#include "eckit/log/RotationTarget.h"

#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/TimeStamp.h"
#include "eckit/runtime/Main.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/StaticMutex.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static StaticMutex local_mutex;

static std::ofstream* last = nullptr;
static time_t lastTime     = 0;

//----------------------------------------------------------------------------------------------------------------------

static std::ostream& rotout() {

    time_t now = ::time(0) / 86400;

    if (now != lastTime || last == nullptr) {

        static std::string logfileFormat = Resource<std::string>("logfileFormat", "~/log/%Y-%m-%d/out");

        TimeStamp ts(logfileFormat);
        PathName path(ts);
        path.mkdir(0777);

        std::ostringstream os;
        os << path << "/" << Main::instance().name();

        delete last;

        /// @todo Find a way to set the close on exec flags
        last = new std::ofstream(os.str().c_str(), std::ios::app);

        lastTime = now;
    }

    return *last;
}

//----------------------------------------------------------------------------------------------------------------------

RotationTarget::RotationTarget() {}

RotationTarget::~RotationTarget() {}

void RotationTarget::write(const char* start, const char* end) {
    AutoLock<StaticMutex> lock(local_mutex);
    rotout().write(start, end - start);
}
void RotationTarget::flush() {
    AutoLock<StaticMutex> lock(local_mutex);
    rotout().flush();
}

void RotationTarget::print(std::ostream& s) const {
    static std::string logfileFormat = Resource<std::string>("logfileFormat", "~/log/%Y-%m-%d/out");
    s << "RotationTarget(format=" << logfileFormat << ")";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
