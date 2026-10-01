// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

ResourceUsage::ResourceUsage() : name_("unnamed"), out_(std::cout) {
    init();
}

ResourceUsage::ResourceUsage(const std::string& name, std::ostream& o) : name_(name), out_(o) {
    init();
}

ResourceUsage::ResourceUsage(const char* name, std::ostream& o) : name_(name), out_(o) {
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
