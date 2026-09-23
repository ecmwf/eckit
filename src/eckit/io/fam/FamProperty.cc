// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

#include "eckit/io/fam/FamProperty.h"

#include <sys/types.h>
#include <unistd.h>

#include <cstdint>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>

#include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

namespace {

fam::perm_t stringToPerm(const std::string& perm) {
    std::size_t pos  = 0;
    const auto value = std::stoul(perm, &pos, 8);

    // reject partial parse (e.g. "0644abc") and values beyond valid permissions
    ASSERT_MSG(pos == perm.size(), "invalid permission string: " + perm);
    ASSERT_MSG(value <= 07777, "permission value out of range: " + perm);

    return static_cast<fam::perm_t>(value);
}

std::string permToString(fam::perm_t perm) {
    std::ostringstream oss;
    oss << std::oct << perm;
    return oss.str();
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
