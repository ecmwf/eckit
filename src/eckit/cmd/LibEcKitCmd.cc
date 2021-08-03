/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/cmd/LibEcKitCmd.h"

#include "eckit/eckit_version.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

REGISTER_LIBRARY(LibEcKitCmd);

LibEcKitCmd::LibEcKitCmd() :
    Library("eckit_cmd") {}

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
