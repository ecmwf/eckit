/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date Apr 2015

#include <sys/resource.h>

#include "eckit/eckit_ecbuild_config.h"
#include "eckit/parser/StringTools.h"

#include "eckit/system/ResourceUsage.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {
namespace system {

//----------------------------------------------------------------------------------------------------------------------

ResourceUsage::ResourceUsage()
{
    struct rusage r_usage;
    ::getrusage(RUSAGE_SELF,&r_usage);

    if(StringTools::startsWith(ECKIT_OS_NAME, "Darwin")) {
        maxrss_ = r_usage.ru_maxrss; // ru_masrss is in bytes
    }
    else {
        maxrss_ = 1024 * r_usage.ru_maxrss; // assume ru_masrss is in Kilobytes
    }
}

void ResourceUsage::print(std::ostream &out) const {
    out << "ResourceUsage(maxrss=" << maxrss() << ")";
}

size_t ResourceUsage::maxrss() const
{
    return maxrss_;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace system
} // namespace eckit

