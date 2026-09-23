// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Simon Smart
/// @date   March 2017

#include "eckit/system/SystemInfoFreeBSD.h"

#include <sys/resource.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>  // FreeBSD: must appear before sys/sysctl.h
#include <unistd.h>

#include <climits>
#include <cstdlib>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/system/MemoryInfo.h"

namespace eckit {
namespace system {

//----------------------------------------------------------------------------------------------------------------------

SystemInfoFreeBSD::~SystemInfoFreeBSD() {}

LocalPathName SystemInfoFreeBSD::executablePath() const {
    Buffer buffer(PATH_MAX);

    int mib[4];
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PATHNAME;
    mib[3] = -1;

    size_t size = buffer.size();
    SYSCALL(::sysctl(mib, 4, buffer, &size, NULL, 0));
    std::string path(buffer, size);

    return LocalPathName(path).realName();
}

MemoryInfo SystemInfoFreeBSD::memoryUsage() const {
    // struct rusage usage;
    // SYSCALL(getrusage(RUSAGE_SELF, &usage));
    // return Mem(usage.ru_maxrss * 1024, 0) ;
    MemoryInfo mem;
    return mem;
}

std::string SystemInfoFreeBSD::dynamicLibraryName(const std::string& name) const {
    return std::string("lib") + name + ".so";
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace system
}  // namespace eckit
