// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <map>
#include <string>
#include <vector>

#include "eckit/codec/Metadata.h"
#include "eckit/codec/detail/RecordSections.h"

namespace eckit::codec {

/// Low-level Record information container.
///
/// No big data is kept here, only metadata, and information
/// on how to retrieve data at a later stage
class ParsedRecord {
public:

    RecordHead head;                                           ///< head section of parsed record
    std::vector<std::string> keys;                             ///< Keys of items encoded in parsed record
    std::map<std::string, Metadata> items;                     ///< Items encoded in parsed record
    std::vector<RecordDataIndexSection::Entry> data_sections;  ///< Description of data sections in parsed record

    /// The parse() function needs to be called during the reading of the record and
    /// completes the "items" through introspection of the "data_sections".
    /// It also computes uncompressed data size using available metadata in the "items"
    void parse();
};

}  // namespace eckit::codec
