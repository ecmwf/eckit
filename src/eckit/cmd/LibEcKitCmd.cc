// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/cmd/LibEcKitCmd.h"

#include "eckit/eckit_version.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

REGISTER_LIBRARY(LibEcKitCmd);

LibEcKitCmd::LibEcKitCmd() : Library("eckit_cmd") {}

LibEcKitCmd& LibEcKitCmd::instance() {
    static LibEcKitCmd lib;
    return lib;
}

const void* LibEcKitCmd::addr() const {
    return this;
}

std::string LibEcKitCmd::version() const {
    return eckit_version_str();
}

std::string LibEcKitCmd::gitsha1(unsigned int count) const {
    std::string sha1(eckit_git_sha1());
    if (sha1.empty()) {
        return "not available";
    }

    return sha1.substr(0, std::min(count, 40u));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
