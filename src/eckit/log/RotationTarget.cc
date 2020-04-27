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

class RotationOutputStream {
public:
    static RotationOutputStream& instance() {
        static RotationOutputStream theinstance;
        return theinstance;
    }

    void write(const char* start, const char* end) {
        AutoLock<StaticMutex> lock(local_mutex);
        rotout().write(start, end - start);
    }

    void flush() {
        AutoLock<StaticMutex> lock(local_mutex);
        if (last_)
            last_->flush();
    }

private:
    RotationOutputStream() : logfileFormat_(Resource<std::string>("logfileFormat", "~/log/%Y-%m-%d/out")) {}

    std::ostream& rotout() {

        time_t now = ::time(nullptr) / 86400;

        if (now != lastTime_ || last_ == nullptr) {

            TimeStamp ts(logfileFormat_);
            PathName path(ts);
            path.mkdir(0777);

            std::ostringstream os;
            os << path << "/" << Main::instance().name();

            delete last_;

            /// @todo Find a way to set the close on exec flags
            last_ = new std::ofstream(os.str().c_str(), std::ios::app);

            lastTime_ = now;
        }

        return *last_;
    }


private:  // members
    std::ofstream* last_ = nullptr;
    time_t lastTime_     = 0;

    std::string logfileFormat_;
};

//----------------------------------------------------------------------------------------------------------------------

RotationTarget::RotationTarget() {}

RotationTarget::~RotationTarget() {}

void RotationTarget::write(const char* start, const char* end) {
    RotationOutputStream::instance().write(start, end);
}
void RotationTarget::flush() {
    RotationOutputStream::instance().flush();
}

void RotationTarget::print(std::ostream& s) const {
    static std::string logfileFormat = Resource<std::string>("logfileFormat", "~/log/%Y-%m-%d/out");
    s << "RotationTarget(format=" << logfileFormat << ")";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
