/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/eckit.h"

#include "eckit/os/System.h"
#include "eckit/types/Types.h"

#if defined(eckit_HAVE_DLADDR)

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <dlfcn.h>

#define ECKIT_ADDRTOPATH_WITH_DLADDR

#endif

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


std::string System::addrToPath(const void* addr) {

    std::string result = "/UNKNOWN";

#ifdef ECKIT_ADDRTOPATH_WITH_DLADDR
    Dl_info info;
    info.dli_fname = result.c_str();
    dladdr(addr, &info);
    result = info.dli_fname;
#endif

    /// @todo: what do we do with archives (.a)
    ///  * maybe we rely on env variable set?
    ///  * or rely on etc/path

    return result;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
