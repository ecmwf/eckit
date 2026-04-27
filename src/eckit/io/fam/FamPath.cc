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

#include <ostream>
#include <string>
#include <tuple>
#include <utility>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/fam/FamTypes.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

namespace {

std::tuple<std::string, std::string> parsePath(const std::string& path) {
    const auto names = Tokenizer("/").tokenize(path);
    const auto count = names.size();
    if (count > 2) {
        throw UserError("Invalid FAM path: " + path, Here());
    }
    switch (count) {
        case 1:
            return {names[0], ""};
        case 2:
            return {names[0], names[1]};
        default:
            return {};
    }
}

/* ISO Object Identifier Namespace */
const uuid_t ns_oid = {0x6b, 0xa7, 0xb8, 0x12, 0x9d, 0xad, 0x11, 0xd1, 0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8};

std::string generateUuid(const std::string& name) {
    std::string result = "00000000-0000-0000-0000-000000000000";

    uuid_t oid;
    uuid_generate_sha1(&oid[0], &ns_oid[0], name.c_str(), name.length());
    uuid_unparse(&oid[0], result.data());

    return result;
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

FamPath::FamPath(std::string region, std::string object) :
    regionName_{std::move(region)}, objectName_{std::move(object)} {}

FamPath::FamPath(const std::string& path) {
    std::tie(regionName_, objectName_) = parsePath(path);
}

FamPath::FamPath(const char* path) : FamPath(std::string(path)) {}

FamPath::FamPath(const URI& uri) : FamPath(uri.name()) {
    ASSERT(uri.scheme() == fam::scheme);
}

FamPath::FamPath(Stream& stream) {
    stream >> regionName_;
    stream >> objectName_;
}

bool FamPath::operator==(const FamPath& other) const {
    return (regionName_ == other.regionName_ && objectName_ == other.objectName_);
}

std::string FamPath::generateUUID() const {
    return generateUuid(asString());
}

void FamPath::encode(Stream& stream) const {
    stream << regionName_;
    stream << objectName_;
}

std::string FamPath::asString() const {
    return objectName_.empty() ? '/' + regionName_ : '/' + regionName_ + '/' + objectName_;
}

std::ostream& operator<<(std::ostream& out, const FamPath& path) {
    out << path.asString();
    return out;
}

Stream& operator<<(Stream& stream, const FamPath& name) {
    name.encode(stream);
    return stream;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
