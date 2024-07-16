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

/// @file   FamPath.h
/// @author Metin Cakircali
/// @date   Jun 2024

#pragma once

#include <iosfwd>
#include <string>

namespace eckit {

class URI;
class Stream;

//----------------------------------------------------------------------------------------------------------------------

struct FamPath {
    static constexpr const auto scheme = "fam";

    static auto generateUUID(const std::string& name) -> std::string;

    FamPath() = default;

    FamPath(std::string region, std::string object);

    FamPath(const std::string& path);

    FamPath(const char* path);

    FamPath(const URI& uri);

    FamPath(Stream& stream);

    bool operator==(const FamPath& other) const;

    auto generateUUID() const -> std::string;

    void encode(Stream& stream) const;

    auto asString() const -> std::string;

    friend auto operator<<(std::ostream& out, const FamPath& path) -> std::ostream&;

    friend auto operator<<(Stream& stream, const FamPath& name) -> Stream&;

    std::string regionName;
    std::string objectName;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
