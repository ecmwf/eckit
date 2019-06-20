/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <signal.h>
#include <unistd.h>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/os/AutoAlarm.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

bool AutoAlarm::caught_ = false;
bool AutoAlarm::throw_  = false;
int AutoAlarm::sec_     = 0;

void AutoAlarm::sigAlarm(int) {
    Log::error() << "Alarm signal received" << std::endl;
    caught_ = true;
    if (throw_)
        throw TimeOut("AutoAlarm", sec_);
}

AutoAlarm::AutoAlarm(int sec, bool t) {
    /// @todo change this to sigaction

    old_       = ::signal(SIGALRM, sigAlarm);
    saveThrow_ = throw_;
    saveSec_   = sec_;
    throw_     = t;
    sec_       = sec;
    caught_    = false;

    ::alarm(sec);
}

AutoAlarm::~AutoAlarm() {
    throw_ = saveThrow_;
    sec_   = saveSec_;
    ::signal(SIGALRM, old_);
    ::alarm(0);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
