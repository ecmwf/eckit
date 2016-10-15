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
#include "eckit/os/Malloc.h"

struct mem {
    size_t resident_size_;
    size_t virtual_size_;
    mem(size_t resident_size, size_t virtual_size):
        resident_size_(resident_size),
        virtual_size_(virtual_size) {}
};


// TODO: move logic to ecbuild
#ifdef __APPLE__
#include <mach/mach.h>

static mem get_mem() {

    struct task_basic_info info;
    mach_msg_type_number_t size = sizeof(info);

    kern_return_t err = task_info(mach_task_self(),
                                  TASK_BASIC_INFO,
                                  (task_info_t)&info,
                                  &size);

    if ( err != KERN_SUCCESS ) {
        throw eckit::FailedSystemCall(mach_error_string(err), Here());
    }

    return mem(info.resident_size, info.virtual_size);

}

#else
static mem get_mem() {
    struct rusage usage;
    SYSCALL(getrusage(RUSAGE_SELF, &usage));
    return mem(usage.ru_maxrss * 1024, 0) ;
}
#endif


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ResourceUsage::ResourceUsage():
    name_("unnamed"),
    out_( std::cout )
{
    init();
}

ResourceUsage::ResourceUsage(const std::string& name, std::ostream& o ):
    name_(name),
    out_(o)
{
    init();
}

ResourceUsage::ResourceUsage(const char* name, std::ostream& o ):
    name_(name),
    out_(o)
{
    init();
}

void ResourceUsage::init() {
    rss_ = get_mem().resident_size_;
    malloc_ = Malloc::allocated();

    out_ << name_ << " => resident size: "
         << eckit::Bytes(rss_);
    out_ << ", allocated: "
         << eckit::Bytes(malloc_);
    out_ << std::endl;
}

ResourceUsage::~ResourceUsage()
{

    unsigned long long rss = get_mem().resident_size_;
    unsigned long long malloc = Malloc::allocated();

    out_ << name_ << " <= resident size: "
         << eckit::Bytes(rss);

    if ( rss > rss_) {
        out_ << " (+" << eckit::Bytes(rss - rss_) << ")";
    }

    if ( rss < rss_) {
        out_ << " (-" << eckit::Bytes(rss_ - rss) << ")";
    }

    out_ << ", allocated: "
         << eckit::Bytes(malloc);

    if ( malloc > malloc_) {
        out_ << " (+" << eckit::Bytes(malloc - malloc_) << ")";
    }

    if ( malloc < malloc_) {
        out_ << " (-" << eckit::Bytes(malloc_ - malloc) << ")";
    }
    out_ << std::endl;
}


//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
