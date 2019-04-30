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

#include "eckit/system/ResourceUsage.h"

#include "eckit/eckit_ecbuild_config.h"
#include "eckit/utils/StringTools.h"

#include "eckit/log/BigNum.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Seconds.h"

#include "eckit/exception/Exceptions.h"

namespace eckit {
namespace system {

//----------------------------------------------------------------------------------------------------------------------

ResourceUsage::ResourceUsage() {
    ::getrusage(RUSAGE_SELF, &usage_);

    if (StringTools::startsWith(ECKIT_OS_NAME, "Darwin")) {
        factor_ = 1;  // ru_masrss is in bytes
    }
    else {
        factor_ = 1024;  // assume ru_masrss is in Kilobytes
    }
}

void ResourceUsage::print(std::ostream& out) const {
    out << "CPU: " << cpuTime() << " (" << eckit::Seconds(cpuTime()) << "), memory: " << maxResidentSetSize() << " ("
        << eckit::Bytes(maxResidentSetSize()) << "), swaps: " << eckit::BigNum(numberOfSwaps());
}

size_t ResourceUsage::maxResidentSetSize() const {
    return usage_.ru_maxrss * factor_;
}

double ResourceUsage::cpuTime() const {
    return usage_.ru_utime.tv_sec + usage_.ru_utime.tv_usec / 1000000.0;
}

size_t ResourceUsage::numberOfSwaps() const {
    return usage_.ru_nswap;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace system
}  // namespace eckit
