/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

#include "eckit/io/fam/FamPath.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/log/Log.h"
#include "eckit/utils/Tokenizer.h"

#include <iostream>
#include <tuple>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

namespace {

auto parsePath(const std::string& path) -> std::tuple<std::string, std::string> {
    const auto names = Tokenizer("/").tokenize(path);
    switch (names.size()) {
        case 1:  return {names[0], ""}; break;
        case 2:  return {names[0], names[1]}; break;
        default: return {}; break;
    }
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

FamPath::FamPath(const std::string& path) {
    std::tie(regionName, objectName) = parsePath(path);
}

FamPath::FamPath(const URI& uri): FamPath(uri.name()) {
    ASSERT(uri.scheme() == SCHEME);
}

std::ostream& operator<<(std::ostream& out, const FamPath& path) {
    out << path.asString();
    return out;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
