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
#include "eckit/net/IPAddress.h"

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
    if (software_.empty() && swVersion_.empty() && enterpriseId_.empty()) {
        return std::string(1, nilvalue());
    }
    // RFC 5424 section 6.3 (only origin)
    std::ostringstream s;
    std::string ip = net::IPAddress::myIPAddress().asString();

    s << "[origin ip=\"" << ip << "\"";
    if (!enterpriseId_.empty()) {
        s << " enterpriseId=\"" << enterpriseId_ << "\"";
    }
    if (!software_.empty()) {
        s << " software=\"" << software_ << "\"";
        if (!swVersion_.empty()) {
            s << " swVersion=\"" << swVersion_ << "\"";
        }
    }
    s << "]";

    return s.str();
}

SysLog::operator std::string() const {

    std::ostringstream os;

    static char sep = ' ';

    os  // RFC 5424 section 6.2 (Header)
        << "<" << priority() << ">" << version() << sep << timestamp() << sep << fqdn() << sep << appName() << sep
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
