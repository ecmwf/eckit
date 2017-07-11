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

#include <unistd.h>
#include <malloc.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <climits>
#include <cstdlib>

#include "eckit/system/SystemInfoLinux.h"

#include "eckit/memory/MemoryBuffer.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/LocalPathName.h"

namespace eckit {
namespace system {

//----------------------------------------------------------------------------------------------------------------------

SystemInfoLinux::~SystemInfoLinux() {
}

LocalPathName SystemInfoLinux::executablePath() const
{
    MemoryBuffer buffer(PATH_MAX);
    ssize_t size = SYSCALL(::readlink("/proc/self/exe", buffer, buffer.size()));
    std::string path(buffer, size);
    return LocalPathName(path).realName();
}

Mem SystemInfoLinux::memoryUsage() const {
    struct rusage usage;
    SYSCALL(getrusage(RUSAGE_SELF, &usage));

    std::ostringstream oss;
    oss << "/proc/" << ::getpid() << "/maps";

    std::ifstream in(oss.str().c_str());
    char line[10240] = {0,};
    size_t shared = 0;

    while (in.getline(line, sizeof(line)-1)) {

        char *p = line;
        while(*p) {
            if(*p == '-') {
                *p = ' ';
                }
                p++;
            }

        std::istringstream iss(line);


        size_t begin, end;
        std::string perms;

        iss >> std::hex >> begin >> end >> perms;

        // std::cout << begin << " " b<< end << " - "  << perms << std::endl;



        if(!perms.empty() && perms[perms.size()-1] == 's') {
            shared += end - begin;
        }
    }

    // std::cout << shared << std::endl;


    return Mem(usage.ru_maxrss * 1024, 0, shared) ;
}

size_t SystemInfoLinux::memoryAllocated() const {
    return mallinfo().uordblks;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace system
} // namespace eckit

