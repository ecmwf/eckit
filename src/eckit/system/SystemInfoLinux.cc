/*
 * (C) Copyright 1996- ECMWF.
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

#include <malloc.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

#include <algorithm>
#include <climits>
#include <cstdlib>
#include <fstream>

#include "eckit/system/SystemInfoLinux.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/memory/MemoryBuffer.h"
#include "eckit/system/MemoryInfo.h"

namespace eckit {
namespace system {

//----------------------------------------------------------------------------------------------------------------------

SystemInfoLinux::~SystemInfoLinux() {}

LocalPathName SystemInfoLinux::executablePath() const {
    MemoryBuffer buffer(PATH_MAX);
    ssize_t size = SYSCALL(::readlink("/proc/self/exe", buffer, buffer.size()));
    std::string path(buffer, size);
    return LocalPathName(path).realName();
}

void SystemInfoLinux::dumpProcMemInfo(std::ostream& os, const char* prepend) const {
    std::ostringstream oss;
    oss << "/proc/" << ::getpid() << "/smaps";
    std::ifstream in(oss.str().c_str());
    char line[10240] = {};
    while (in.getline(line, sizeof(line) - 1)) {
        os << prepend << ' ' << line << std::endl;
    }
}

void SystemInfoLinux::dumpSysMemInfo(std::ostream& os, const char* prepend) const {
    std::ostringstream oss;
    oss << "/proc/meminfo";
    std::ifstream in(oss.str().c_str());
    char line[10240] = {};
    while (in.getline(line, sizeof(line) - 1)) {
        os << prepend << ' ' << line << std::endl;
    }
}

std::string SystemInfoLinux::dynamicLibraryName(const std::string& name) const {
    return std::string("lib") + name + ".so";
}

MemoryInfo SystemInfoLinux::memoryUsage() const {


    struct rusage usage;
    SYSCALL(getrusage(RUSAGE_SELF, &usage));

    static const char* debug = getenv("ECKIT_SYSINFO_DEBUG");

    if (debug && atoi(debug)) {
        dumpSysMemInfo(eckit::Log::info(), debug);
    }

    std::ostringstream oss;
    oss << "/proc/" << ::getpid() << "/maps";

    std::ifstream in(oss.str().c_str());
    char line[10240] = {};

    size_t shared  = 0;
    size_t read    = 0;
    size_t write   = 0;
    size_t execute = 0;
    size_t privy   = 0;

    std::map<std::string, size_t> s;

    while (in.getline(line, sizeof(line) - 1)) {

        std::istringstream in1(line);
        std::string range, perms;
        in1 >> range >> perms;

        std::replace(range.begin(), range.end(), '-', ' ');
        std::istringstream in2(range);

        size_t begin, end;

        in2 >> std::hex >> begin >> end;
        // std::cout << begin << " " b<< end << " - "  << perms << std::endl;

        if (!perms.empty()) {

            // http://man7.org/linux/man-pages/man5/proc.5.html

            switch (perms[perms.size() - 1]) {

                case 'r':
                    read += end - begin;
                    break;
                case 'w':
                    write += end - begin;
                    break;
                case 'x':
                    execute += end - begin;
                    break;
                case 's':
                    shared += end - begin;
                    break;
                case 'p':
                    privy += end - begin;
                    break;
            }
        }
    }

    MemoryInfo mem;

    mem.resident_size_  = usage.ru_maxrss * 1024;
    mem.mapped_shared_  = shared;
    mem.mapped_read_    = read;
    mem.mapped_write_   = write;
    mem.mapped_execute_ = execute;
    mem.mapped_private_ = privy;

    struct mallinfo m = mallinfo();
    mem.arena_        = m.arena;
    // mem.ordblks_ = m.ordblks;
    // mem.smblks_ = m.smblks;
    // mem.hblks_ = m.hblks;
    mem.hblkhd_   = m.hblkhd;
    mem.usmblks_  = m.usmblks;
    mem.fsmblks_  = m.fsmblks;
    mem.uordblks_ = m.uordblks;
    mem.fordblks_ = m.fordblks;
    mem.keepcost_ = m.keepcost;

    return mem;
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace system
}  // namespace eckit
