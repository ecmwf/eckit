/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


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
