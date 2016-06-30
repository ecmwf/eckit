/*
 * (C) Copyright 1996-2016 ECMWF.
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

// #ifdef LINUX
// #define _GNU_SOURCE
// #endif

#include <dlfcn.h>

// #ifdef LINUX

// typedef struct {
//         const char *dli_fname;
//         void       *dli_fbase;
//         const char *dli_sname;
//         void       *dli_saddr;
// } Dl_info;
// #endif

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


// extern "C" int dladdr(const void*, Dl_info*);


std::string System::addrToPath(const void *addr) {
    Dl_info info;
    info.dli_fname = "/UNKNOWN";
    dladdr(addr, &info);
    return std::string(info.dli_fname);
}

//-----------------------------------------------------------------------------

} // namespace eckit

