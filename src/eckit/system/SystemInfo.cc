/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   May 2016

#include "eckit/system/SystemInfo.h"

#include "eckit/eckit_ecbuild_config.h"
#include "eckit/parser/StringTools.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/memory/ScopedPtr.h"

namespace eckit {
namespace system {

//----------------------------------------------------------------------------------------------------------------------

class SystemInfoMacOSX : public SystemInfo {

    virtual eckit::PathName executablePath() const {
        NOTIMP;
    }

};

//----------------------------------------------------------------------------------------------------------------------

class SystemInfoLinux : public SystemInfo {

    virtual eckit::PathName executablePath() const {
        NOTIMP;
    }

};

//----------------------------------------------------------------------------------------------------------------------

SystemInfo* makeSystemInfo(const std::string& system)
{
    ///< @todo add a factory?

    if (StringTools::startsWith(ECKIT_OS_NAME, "Darwin")) {
        return new SystemInfoMacOSX();
    }

    if (StringTools::startsWith(ECKIT_OS_NAME, "Linux")) {
        return new SystemInfoLinux();
    }

    NOTIMP;
}

static pthread_once_t once  = PTHREAD_ONCE_INIT;
static eckit::ScopedPtr<SystemInfo> systemInfoPtr;

static void createInstance() {
    ASSERT(!systemInfoPtr);
    systemInfoPtr.reset( makeSystemInfo(ECKIT_OS_NAME) );
}

//----------------------------------------------------------------------------------------------------------------------

const SystemInfo& SystemInfo::instance() {
    pthread_once(&once, createInstance);
    ASSERT(systemInfoPtr);
    return *systemInfoPtr;
}

SystemInfo::~SystemInfo() {
}

void SystemInfo::print(std::ostream &out) const {
    out << "SystemInfo("
        << "executablePath=" << executablePath()
        << ")";
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace system
} // namespace eckit

