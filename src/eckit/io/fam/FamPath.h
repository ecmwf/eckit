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

class FamPath {
public:

    FamPath() = default;

    FamPath(std::string region, std::string object);

    explicit FamPath(const std::string& path);

    explicit FamPath(const char* path);

    explicit FamPath(const URI& uri);

    explicit FamPath(Stream& stream);

    bool operator==(const FamPath& other) const;

    std::string generateUUID() const;

    void encode(Stream& stream) const;

    std::string asString() const;

    auto regionName() const -> const std::string& { return regionName_; }
    auto objectName() const -> const std::string& { return objectName_; }

    void regionName(std::string name) { regionName_ = std::move(name); }
    void objectName(std::string name) { objectName_ = std::move(name); }

private:

    friend std::ostream& operator<<(std::ostream& out, const FamPath& path);

    friend Stream& operator<<(Stream& stream, const FamPath& name);

private:  // members

    std::string regionName_;
    std::string objectName_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
