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

#include <unistd.h>

#include <climits>
#include <cstdlib>

#include "eckit/system/SystemInfoLinux.h"

#include "eckit/io/Buffer.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {
namespace system {

//----------------------------------------------------------------------------------------------------------------------

SystemInfoLinux::~SystemInfoLinux() {
}

PathName SystemInfoLinux::executablePath() const
{
    Buffer buffer(MAXPATHLEN);
	ssize_t size = SYSCALL(::readlink("/proc/self/exe", buffer, buffer.size()));
    std::string path(buffer, size);
    return PathName(path).realName();
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace system
} // namespace eckit

