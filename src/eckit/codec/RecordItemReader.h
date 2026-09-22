// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

#include "eckit/codec/Record.h"
#include "eckit/codec/RecordItem.h"
#include "eckit/codec/Stream.h"

namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

class RecordItemReader {
public:

    RecordItemReader(Stream, size_t offset, const std::string& key);

    RecordItemReader(Stream, const std::string& key);

    explicit RecordItemReader(const std::string& uri);

    void read(RecordItem& item);

    void read(Metadata&, bool follow_links = true);

    void read(Metadata&, Data&);

private:

    RecordItemReader(const std::string& ref, const std::string& uri);

    Stream in_;
    Record record_;

    std::string ref_{};  // directory to which relative URI's are evaluated
    RecordItem::URI uri_;
};

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
