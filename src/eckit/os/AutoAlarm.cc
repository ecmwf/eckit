// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <unistd.h>
#include <csignal>

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
    if (throw_) {
        throw TimeOut("AutoAlarm", sec_);
    }
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
