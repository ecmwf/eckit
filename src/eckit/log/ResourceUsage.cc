/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Seconds.h"
#include "eckit/log/ResourceUsage.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Bytes.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ResourceUsage::ResourceUsage():
    name_("unnamed"),
    out_( std::cout )
{
    SYSCALL(getrusage(RUSAGE_SELF, &usage_));
}

ResourceUsage::ResourceUsage(const std::string& name, std::ostream& o ):
    name_(name),
    out_(o)
{
    SYSCALL(getrusage(RUSAGE_SELF, &usage_));
}

ResourceUsage::ResourceUsage(const char* name, std::ostream& o ):
    name_(name),
    out_(o)
{
    SYSCALL(getrusage(RUSAGE_SELF, &usage_));
}


ResourceUsage::~ResourceUsage()
{
    struct rusage  usage;
    SYSCALL(getrusage(RUSAGE_SELF, &usage));

    out_ << name_ << " resident size "
         << eckit::Bytes(usage.ru_maxrss * 1024);

    if ( usage.ru_maxrss >  usage_.ru_maxrss) {

        out_  << ", increase: " << eckit::Bytes((usage.ru_maxrss -  usage_.ru_maxrss) * 1024);
    }

    if ( usage.ru_maxrss <  usage_.ru_maxrss) {
        out_  << ", decrease: " << eckit::Bytes((usage_.ru_maxrss -  usage.ru_maxrss) * 1024);
    }

    out_ << std::endl;
}


//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
