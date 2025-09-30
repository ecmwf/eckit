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

#include <uuid/uuid.h>

#include <iostream>
#include <string>
#include <tuple>
#include <utility>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

namespace {

auto parsePath(const std::string& path) -> std::tuple<std::string, std::string> {
    const auto names = Tokenizer("/").tokenize(path);
    switch (names.size()) {
        case 1:
            return {names[0], ""};
            break;
        case 2:
            return {names[0], names[1]};
            break;
        default:
            return {};
            break;
    }
}

/* ISO Object Identifier Namespace */
const uuid_t ns_oid = {0x6b, 0xa7, 0xb8, 0x12, 0x9d, 0xad, 0x11, 0xd1, 0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8};

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

auto FamPath::generateUUID(const std::string& name) -> std::string {
    std::string result = "00000000-0000-0000-0000-000000000000";

    uuid_t oid;
    uuid_generate_sha1(&oid[0], &ns_oid[0], name.c_str(), name.length());
    uuid_unparse(&oid[0], result.data());

    return result;
}

//----------------------------------------------------------------------------------------------------------------------

FamPath::FamPath(std::string region, std::string object) :
    regionName{std::move(region)}, objectName{std::move(object)} {}

FamPath::FamPath(const std::string& path) {
    std::tie(regionName, objectName) = parsePath(path);
}

FamPath::FamPath(const char* path) : FamPath(std::string(path)) {}

FamPath::FamPath(const URI& uri) : FamPath(uri.name()) {
    ASSERT(uri.scheme() == scheme);
}

FamPath::FamPath(Stream& stream) {
    stream >> regionName;
    stream >> objectName;
}

bool FamPath::operator==(const FamPath& other) const {
    return (regionName == other.regionName && objectName == other.objectName);
}

auto FamPath::generateUUID() const -> std::string {
    return generateUUID(regionName + objectName);
}

void FamPath::encode(Stream& stream) const {
    stream << regionName;
    stream << objectName;
}

auto FamPath::asString() const -> std::string {
    return objectName.empty() ? '/' + regionName : '/' + regionName + '/' + objectName;
}

auto operator<<(std::ostream& out, const FamPath& path) -> std::ostream& {
    out << path.asString();
    return out;
}

auto operator<<(Stream& stream, const FamPath& name) -> Stream& {
    name.encode(stream);
    return stream;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
