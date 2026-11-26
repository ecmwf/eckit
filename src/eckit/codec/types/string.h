// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


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
