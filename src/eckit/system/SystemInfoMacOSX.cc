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

#include <sys/param.h>
#include <mach-o/dyld.h>

#include "eckit/system/SystemInfoMacOSX.h"

#include "eckit/io/Buffer.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {
namespace system {

//----------------------------------------------------------------------------------------------------------------------

SystemInfoMacOSX::~SystemInfoMacOSX() {
}

PathName SystemInfoMacOSX::executablePath() const
{
    Buffer buffer(MAXPATHLEN);

    int err = 0;
    uint32_t actual = uint32_t(buffer.size());
    if( (err = _NSGetExecutablePath(buffer, &actual)) == -1 ) {
        buffer.resize(actual);
        err = _NSGetExecutablePath(buffer, &actual);
    }

    if(err != 0) {
        std::ostringstream oss;
        oss << "_NSGetExecutablePath when called with buffer sized " << buffer.size();
        throw FailedSystemCall(oss.str(), Here());
    }

    std::string path(buffer);

    Log::debug() << "Executable path is [" << path << "]" << std::endl;

    return PathName(path).realName();
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace system
} // namespace eckit

