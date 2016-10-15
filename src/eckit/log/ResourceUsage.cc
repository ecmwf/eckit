/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>

#include "eckit/log/Seconds.h"
#include "eckit/log/ResourceUsage.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Bytes.h"


// TODO: move logic to ecbuild
#ifdef __APPLE__
static unsigned long long to_bytes() {
    double clock_ticks = sysconf(_SC_CLK_TCK);
    struct rusage usage;
    SYSCALL(getrusage(RUSAGE_SELF, &usage));
    double utime = (double)usage.ru_utime.tv_sec + ((double)usage.ru_utime.tv_usec / 1000000.);
    double stime = (double)usage.ru_stime.tv_sec + ((double)usage.ru_stime.tv_usec / 1000000.);

    double ticks = (utime + stime) * clock_ticks;
    return usage.ru_maxrss / ticks;
}
#else
static unsigned long long  to_bytes(size_t size) {
    struct rusage usage;
    SYSCALL(getrusage(RUSAGE_SELF, &usage));
    return usage.ru_maxrss * 1024;
}
#endif

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ResourceUsage::ResourceUsage():
    name_("unnamed"),
    out_( std::cout )
{
    usage_ = to_bytes();
}

ResourceUsage::ResourceUsage(const std::string& name, std::ostream& o ):
    name_(name),
    out_(o)
{
    usage_ = to_bytes();
}

ResourceUsage::ResourceUsage(const char* name, std::ostream& o ):
    name_(name),
    out_(o)
{

    usage_ = to_bytes();
}


ResourceUsage::~ResourceUsage()
{

    unsigned long long current = to_bytes();

    out_ << name_ << " resident size "
         << eckit::Bytes(current);

    if ( current >  usage_) {

        out_  << ", increase: " << eckit::Bytes(current -  usage_);
    }

    if ( current <  usage_) {
        out_  << ", decrease: " << eckit::Bytes(usage_ -  current);
    }

    out_ << std::endl;
}


//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
