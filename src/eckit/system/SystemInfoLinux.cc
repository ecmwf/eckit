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

void SystemInfoLinux::dumpProcMemInfo(std::ostream& os, const char* prepend) const
{
    std::ostringstream oss;
    oss << "/proc/" << ::getpid() << "/smaps";
    std::ifstream in(oss.str().c_str());
    char line[10240] = {0,};
    while (in.getline(line, sizeof(line) - 1)) {
        if(prepend) os << prepend << ' ';
        os << line << std::endl;
    }
}

void SystemInfoLinux::dumpSysMemInfo(std::ostream& os, const char* prepend) const
{
    std::ostringstream oss;
    oss << "/proc/meminfo";
    std::ifstream in(oss.str().c_str());
    char line[10240] = {0,};
    while (in.getline(line, sizeof(line) - 1)) {
        if(prepend) os << prepend << ' ';
        os << line << std::endl;
    }
}

Mem SystemInfoLinux::memoryUsage() const {

    struct rusage usage;
    SYSCALL(getrusage(RUSAGE_SELF, &usage));

    static const char* debug = getenv("ECKIT_SYSINFO_DEBUG");

    if (debug && atoi(debug)) {
            dumpSysMemInfo(eckit::Log::info(), debug);
    }

    std::ostringstream oss;
    oss << "/proc/" << ::getpid() << "/maps";

    std::ifstream in(oss.str().c_str());
    char line[10240] = {0,};

    size_t shared = 0;
    size_t read = 0;
    size_t write = 0;
    size_t execute = 0;
    size_t privy = 0;

    std::map<std::string, size_t> s;



    while (in.getline(line, sizeof(line) - 1)) {




        std::istringstream in1(line);
        std::string range, perms;
        in1 >> range >> perms;

        std::replace( range.begin(), range.end(), '-', ' ');
        std::istringstream in2(range);

        size_t begin, end;

        in2 >> std::hex >> begin >> end;
        // std::cout << begin << " " b<< end << " - "  << perms << std::endl;

        if (!perms.empty()) {

            // http://man7.org/linux/man-pages/man5/proc.5.html

            switch (perms[perms.size() - 1]) {

            case 'r': read += end - begin; break;
            case 'w': write += end - begin; break;
            case 'x': execute += end - begin; break;
            case 's': shared += end - begin; break;
            case 'p': privy += end - begin; break;

            }
        }
    }


    // std::cout << shared << std::endl

    return Mem(usage.ru_maxrss * 1024,
               0,
               shared,
               read,
               write,
               execute,
               privy) ;
}

size_t SystemInfoLinux::memoryAllocated() const {
    return mallinfo().uordblks;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace system
} // namespace eckit

