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

/// @file   FamProperty.h
/// @author Metin Cakircali
/// @date   Mar 2024

#pragma once

#include <ostream>
#include <string>

#include "eckit/io/fam/FamTypes.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

struct FamProperty {

    static constexpr fam::perm_t default_perm = 0640;

    FamProperty() = default;

    FamProperty(fam::size_t size, fam::perm_t perm, std::string name, std::uint32_t uid, std::uint32_t gid);

    FamProperty(fam::size_t size, fam::perm_t perm, const std::string& name);

    FamProperty(fam::size_t size, fam::perm_t perm);

    FamProperty(fam::size_t size, const std::string& perm);

    void print(std::ostream& out) const;

    bool operator==(const FamProperty& other) const {
        return (size == other.size && perm == other.perm && name == other.name && uid == other.uid && gid == other.gid);
    }

    friend std::ostream& operator<<(std::ostream& out, const FamProperty& prop);

    fam::size_t size{0};
    fam::perm_t perm{default_perm};
    std::string name;
    std::uint32_t uid{0};
    std::uint32_t gid{0};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
