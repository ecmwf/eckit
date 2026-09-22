// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   May 2016

#include "eckit/system/ResourceUsage.h"

#include "eckit/eckit_ecbuild_config.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/BigNum.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Seconds.h"
#include "eckit/utils/StringTools.h"

namespace eckit::system {

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
    return usage_.ru_utime.tv_sec + (usage_.ru_utime.tv_usec / eckit::Seconds::usec_per_sec);
}

double ResourceUsage::totalCpuTime() const {
    return usage_.ru_utime.tv_sec + (usage_.ru_utime.tv_usec / eckit::Seconds::usec_per_sec) + usage_.ru_stime.tv_sec +
           (usage_.ru_stime.tv_usec / eckit::Seconds::usec_per_sec);
}

size_t ResourceUsage::numberOfSwaps() const {
    return usage_.ru_nswap;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::system
