// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   August 2016

#include <cstdlib>

#include <algorithm>
#include <string>

#include "eckit/config/LibEcKit.h"

#include "eckit/config/Resource.h"
#include "eckit/eckit_version.h"
#include "eckit/thread/AutoLock.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

REGISTER_LIBRARY(LibEcKit);

LibEcKit::LibEcKit() : Library("eckit"), abort_handler_(&(::abort)), dontDeregisterFactories_(false) {
    // can't use Resource here (too early in the initialisation)
    dontDeregisterFactories_ = (::getenv("ECKIT_DONT_DEREGISTER_FACTORIES") != nullptr);
}

LibEcKit& LibEcKit::instance() {
    static LibEcKit libeckit;
    return libeckit;
}

void LibEcKit::setAbortHandler(abort_handler_t h) {
    AutoLock<LibEcKit> lock(*this);
    if (h) {
        abort_handler_ = h;
    }
}

void LibEcKit::abort() {
    abort_handler_();
}

bool LibEcKit::dontDeregisterFactories() const {
    return dontDeregisterFactories_;
}

const void* LibEcKit::addr() const {
    return this;
}

std::string LibEcKit::version() const {
    return eckit_version_str();
}

std::string LibEcKit::gitsha1(unsigned int count) const {
    std::string sha1(eckit_git_sha1());
    if (sha1.empty()) {
        return "not available";
    }

    return sha1.substr(0, std::min(count, 40u));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
