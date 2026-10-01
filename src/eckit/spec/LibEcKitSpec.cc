// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/spec/LibEcKitSpec.h"

#include "eckit/eckit_version.h"


namespace eckit {


REGISTER_LIBRARY(LibEcKitSpec);


LibEcKitSpec::LibEcKitSpec() : Library("eckit_spec") {}


LibEcKitSpec& LibEcKitSpec::instance() {
    static LibEcKitSpec lib;
    return lib;
}


const void* LibEcKitSpec::addr() const {
    return this;
}


std::string LibEcKitSpec::version() const {
    return eckit_version_str();
}


std::string LibEcKitSpec::gitsha1(unsigned int count) const {
    std::string sha1(eckit_git_sha1());
    return sha1.empty() ? "not available" : sha1.substr(0, std::min(count, 40U));
}


}  // namespace eckit
