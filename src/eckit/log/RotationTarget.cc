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
#include "eckit/io/Buffer.h"
#include "eckit/log/TimeStamp.h"
#include "eckit/runtime/Main.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/StaticMutex.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RotationOutputStream;

static StaticMutex local_mutex;

class RotationOutputStream {
public:
    static RotationOutputStream& instance(const std::string& name) {
        AutoLock<StaticMutex> lock(local_mutex);

        static std::map<std::string, RotationOutputStream*> instances;

        auto j = instances.find(name);
        if (j == instances.end()) {
            instances[name] = new RotationOutputStream(name);
            j               = instances.find(name);
        }
        return *(*j).second;
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
    RotationOutputStream(const std::string& name) :
        name_(name),
        logfileFormat_(Resource<std::string>("logfileFormat", "~/log/%Y-%m-%d/out")),
        buffer_(0),
        logFilesBufferSize_(Resource<size_t>("logFilesBufferSize", 4 * 1024)) {}

    std::ostream& rotout() {

        time_t now = ::time(nullptr) / 86400;

        if (now != lastTime_ || last_ == nullptr) {

            TimeStamp ts(logfileFormat_);
            PathName path(ts);
            path.mkdir(0777);

            std::ostringstream os;
            os << path << "/" << name_;

            delete last_;

            last_ = new std::ofstream();

            if (logFilesBufferSize_) {
                buffer_.resize(logFilesBufferSize_);
                buffer_.zero();
                last_->rdbuf()->pubsetbuf(buffer_, buffer_.size());
            }

            last_->open(os.str().c_str(), std::ofstream::out | std::ofstream::app);
            if (last_->fail()) {
                throw eckit::CantOpenFile(os.str());
            }

            /// @todo Find a way to set the close on exec flags,
            ///       or to get the file descriptor from ofstream so we can do:
            ///       flags = fcntl(fd, F_GETFD);
            ///       flags |= FD_CLOEXEC;
            ///       fcntl(fd, F_SETFD, flags)

            lastTime_ = now;
        }

        return *last_;
    }


private:  // members
    std::ofstream* last_ = nullptr;
    time_t lastTime_     = 0;

    std::string name_;
    std::string logfileFormat_;
    Buffer buffer_;
    size_t logFilesBufferSize_;
};

//----------------------------------------------------------------------------------------------------------------------

RotationTarget::RotationTarget(const std::string& name) :
    name_(name) {
    if (name_.empty()) {
        name_ = Main::instance().name();
    }
}

RotationTarget::~RotationTarget() {}

void RotationTarget::write(const char* start, const char* end) {
    RotationOutputStream::instance(name_).write(start, end);
}
void RotationTarget::flush() {
    RotationOutputStream::instance(name_).flush();
}

void RotationTarget::print(std::ostream& s) const {
    static std::string logfileFormat = Resource<std::string>("logfileFormat", "~/log/%Y-%m-%d/out");
    s << "RotationTarget(format=" << logfileFormat << ")";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
