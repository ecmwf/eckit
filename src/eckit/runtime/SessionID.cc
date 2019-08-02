/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/runtime/SessionID.h"

#include <unistd.h>
#include <ostream>

#include "eckit/runtime/Main.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/utils/Hash.h"

#include "eckit/log/Log.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

// n.b. thread_ is not uniquely serialisable. So use the hash of it.
// This is only for verification purposes --> Uniquess does not need to be guaranteed

SessionID::SessionID() :
    hostname_(Main::hostname()),
    pid_(::getpid()),
    thread_(std::hash<std::thread::id>()(std::this_thread::get_id())),
    time_(::time(0)) {}

SessionID::SessionID(Stream& s) {
    s >> hostname_;
    s >> pid_;
    unsigned long long thread_tmp;
    s >> thread_tmp;
    s >> time_;

    thread_ = thread_tmp;
}

void SessionID::encode(Stream& s) const {
    s << hostname_;
    s << pid_;
    s << static_cast<unsigned long long>(thread_);
    s << time_;
}

bool SessionID::operator==(const SessionID& rhs) {
    return (pid_ == rhs.pid_ && thread_ == rhs.thread_ && time_ == rhs.time_ && hostname_ == rhs.hostname_);
}

bool SessionID::operator!=(const SessionID& rhs) {
    return !(*this == rhs);
}

void SessionID::hash(Hash& h) const {
    h.add(hostname_);
    h.add(pid_);
    h.add(thread_);
    h.add(time_);
}

void SessionID::print(std::ostream& os) const {
    os << "Session(" << hostname_ << ":" << pid_ << ":" << thread_ << ":" << time_ << ")";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
