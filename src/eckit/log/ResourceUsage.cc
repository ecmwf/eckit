/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/ResourceUsage.h"
#include "eckit/log/Seconds.h"

#include "eckit/memory/MMap.h"
#include "eckit/memory/Shmget.h"
#include "eckit/runtime/Main.h"
#include "eckit/system/SystemInfo.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ResourceUsage::ResourceUsage() :
    name_("unnamed"), out_(std::cout) {
    init();
}

ResourceUsage::ResourceUsage(const std::string& name, std::ostream& o) :
    name_(name), out_(o) {
    init();
}

ResourceUsage::ResourceUsage(const char* name, std::ostream& o) :
    name_(name), out_(o) {
    init();
}

void ResourceUsage::init() {

    using namespace eckit::system;
    const SystemInfo& sysinfo = SystemInfo::instance();

    hostname_ = Main::hostname();

    usage_ = sysinfo.memoryUsage();

    out_ << "ResourceUsage " << name_ << " => " << hostname_ << " " << usage_ << std::endl;
}

ResourceUsage::~ResourceUsage() {
    using namespace eckit::system;
    const SystemInfo& sysinfo = SystemInfo::instance();

    MemoryInfo usage = sysinfo.memoryUsage();

    out_ << "ResourceUsage " << name_ << " <= " << hostname_ << " ";

    usage.delta(out_, usage_);


    out_ << std::endl;
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
