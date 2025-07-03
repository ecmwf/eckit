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

#include "eckit/io/fam/FamProperty.h"

#include <sys/types.h>
#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <ostream>
#include <string>
#include <utility>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

namespace {

fam::perm_t stringToPerm(const std::string& perm) {
    return static_cast<fam::perm_t>(std::stoul(perm, nullptr, 8));
}

std::string permToString(fam::perm_t perm) {
    char buf[5];
    snprintf(buf, sizeof(buf), "%04o", perm);
    return std::string(buf);
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

FamProperty::FamProperty(fam::size_t size, fam::perm_t perm, std::string name, std::uint32_t uid, std::uint32_t gid) :
    size{size}, perm{perm}, name{std::move(name)}, uid{uid}, gid{gid} {}

FamProperty::FamProperty(fam::size_t size, fam::perm_t perm, const std::string& name) :
    FamProperty(size, perm, name, getuid(), getgid()) {}

FamProperty::FamProperty(fam::size_t size, fam::perm_t perm) : FamProperty(size, perm, "") {}

FamProperty::FamProperty(fam::size_t size, const std::string& perm) : FamProperty(size, stringToPerm(perm)) {}

void FamProperty::print(std::ostream& out) const {
    out << "Property[size=" << size << ", perm=" << perm << "(" << permToString(perm) << ")" << ",name=" << name
        << ",uid=" << uid << ",gid=" << gid << "]";
}

//----------------------------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const FamProperty& prop) {
    prop.print(out);
    return out;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
