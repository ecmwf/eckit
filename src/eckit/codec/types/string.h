/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#include <string>

#include "eckit/codec/Data.h"
#include "eckit/codec/Metadata.h"
#include "eckit/exception/Exceptions.h"

namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

inline size_t encode_metadata(const std::string& value, Metadata& out) {
    out.set("type", "string");
    out.set("value", value);
    return 0;
}

inline void encode_data(const std::string&, Data&) {}

inline void decode(const Metadata& metadata, const Data&, std::string& value) {
    ASSERT(metadata.getString("type") == "string");
    metadata.get("value", value);
}

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
