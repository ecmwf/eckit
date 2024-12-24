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
 * This software was developed as part of the Horizon Europe programme funded project DaFab
 * (Grant agreement: 101128693) https://www.dafab-ai.eu/
 */

/// @file   S3ObjectPath.h
/// @author Metin Cakircali
/// @date   Dec 2024

#pragma once

#include <ostream>
#include <string>
#include <utility>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

struct S3ObjectPath {

    S3ObjectPath(std::string bucket, std::string object) : bucket {std::move(bucket)}, object {std::move(object)} { }

    auto asString() const -> std::string { return '/' + bucket + '/' + object; }

    operator std::string() const { return asString(); }

    friend std::ostream& operator<<(std::ostream& out, const S3ObjectPath& path) {
        out << "bucket=" << path.bucket << ",object=" << path.object;
        return out;
    }

    // members

    std::string bucket;
    std::string object;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
