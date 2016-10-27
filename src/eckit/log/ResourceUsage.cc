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

#include "eckit/system/SystemInfo.h"

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

    using namespace eckit::system;
    const SystemInfo& sysinfo = SystemInfo::instance();

    rss_ = sysinfo.memoryUsage().resident_size_;
    malloc_ = sysinfo.memoryAllocated();

    out_ << name_ << " => resident size: "
         << eckit::Bytes(rss_);
    out_ << ", allocated: "
         << eckit::Bytes(malloc_);
    out_ << std::endl;
}

ResourceUsage::~ResourceUsage()
{
    using namespace eckit::system;
    const SystemInfo& sysinfo = SystemInfo::instance();

    size_t rss = sysinfo.memoryUsage().resident_size_;
    size_t malloc = sysinfo.memoryAllocated();

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
