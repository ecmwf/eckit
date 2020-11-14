/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>

#include <sstream>

#include "eckit/log/Log.h"
#include "eckit/log/SysLog.h"
#include "eckit/log/Timer.h"
#include "eckit/runtime/Main.h"

#include "eckit/log/TimeStamp.h"

namespace eckit {


SysLog::SysLog(const std::string& msg, int msgid, Facility f, Severity s) :
    facility_(f), severity_(s), appName_(Main::instance().name()), msgid_(msgid), msg_(msg) {
    timestamp_ = TimeStamp("%Y-%m-%dT%H:%M:%SZ");  ///< assumes we are in UTC
}

std::string SysLog::fqdn() const {
    return Main::hostname();
}

std::string SysLog::appName() const {
    return appName_;
}

void SysLog::appName(const std::string& app) {
    appName_ = app;
}

int SysLog::procid() const {
    return ::getpid();
}

std::string SysLog::structuredData() const {
    std::ostringstream s;

    /// @todo Implement the structured message meta-description as described in RFC5424 secion 6.3
    s << nilvalue();

    return s.str();
}

SysLog::operator std::string() const {

    std::ostringstream os;

    static char sep = ' ';

    os  // RFC 5424 section 6.2 (Header)
        << "<" << priotity() << ">" << version() << sep << timestamp() << sep << fqdn() << sep << appName() << sep
        << procid() << sep << msgid()
        << sep
        // RFC 5424 section 6.3
        << structuredData()
        << sep
        // RFC 5424 section 6.4
        << msg_;

    return os.str();
}

}  // namespace eckit
