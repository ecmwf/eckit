/*
 * (C) Copyright 1996-2017 ECMWF.
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

#include "eckit/filesystem/LocalPathName.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/memory/ScopedPtr.h"

#if defined(__APPLE__) && defined(__MACH__)
#include "eckit/system/SystemInfoMacOSX.h"
#endif

#if defined(__linux__)
#include "eckit/system/SystemInfoLinux.h"
#endif

#if defined(__FreeBSD__)
#include "eckit/system/SystemInfoFreeBSD.h"
#endif

namespace eckit {
namespace system {

//----------------------------------------------------------------------------------------------------------------------

SystemInfo* makeSystemInfo(const std::string& system)
{
    ///< @todo add a factory?

#if defined(__APPLE__) && defined(__MACH__)
    if (StringTools::startsWith(ECKIT_OS_NAME, "Darwin")) { // double check with ecbuild name
        return new SystemInfoMacOSX();
    }
#endif

#if defined(__linux__)
    if (StringTools::startsWith(ECKIT_OS_NAME, "Linux")) {
        return new SystemInfoLinux();
    }
#endif

#if defined(__FreeBSD__)
    if (StringTools::startsWith(ECKIT_OS_NAME, "FreeBSD")) {
        return new SystemInfoFreeBSD();
    }
#endif

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

void SystemInfo::dumpSysMemInfo(std::ostream& os, const char* prepend) const
{
    if(prepend)
        os << prepend;

    os <<  "SystemInfo::dumpSysMemInfo() NOT IMPLEMENTED FOR SYSTEM " << ECKIT_OS_NAME << std::endl;
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

