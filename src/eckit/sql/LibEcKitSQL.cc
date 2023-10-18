/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/LibEcKitSQL.h"

#include "eckit/eckit_version.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

REGISTER_LIBRARY(LibEcKitSQL);

LibEcKitSQL::LibEcKitSQL() :
    Library("eckit_sql"),
    integersAsDoubles_(true),
    fullyQualifyColumnNames_(true) {}

LibEcKitSQL& LibEcKitSQL::instance() {
    static LibEcKitSQL lib;
    return lib;
}

const void* LibEcKitSQL::addr() const {
    return this;
}

std::string LibEcKitSQL::version() const {
    return eckit_version_str();
}

std::string LibEcKitSQL::gitsha1(unsigned int count) const {
    std::string sha1(eckit_git_sha1());
    if (sha1.empty()) {
        return "not available";
    }

    return sha1.substr(0, std::min(count, 40u));
}

bool LibEcKitSQL::treatIntegersAsDoubles() const {
    return integersAsDoubles_;
}

void LibEcKitSQL::treatIntegersAsDoubles(bool flag) {
    integersAsDoubles_ = flag;
}

bool LibEcKitSQL::fullyQualifyColumnNames() const {
    return fullyQualifyColumnNames_;
}

void LibEcKitSQL::fullyQualifyColumnNames(bool flag) {
    fullyQualifyColumnNames_ = flag;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
