// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "eckit/codec/RecordItem.h"
#include "eckit/codec/detail/Decoder.h"

namespace eckit::codec {

//---------------------------------------------------------------------------------------------------------------------

class ReadRequest {
public:

    ReadRequest(ReadRequest&& other);

    template <typename T>
    ReadRequest(Stream stream, size_t offset, const std::string& key, T& value) :
        ReadRequest{stream, offset, key, new Decoder(value)} {}

    template <typename T>
    ReadRequest(const std::string& URI, T& value) : ReadRequest{URI, new Decoder(value)} {}

    template <typename T>
    ReadRequest(const RecordItem::URI& URI, T& value) : ReadRequest{URI.str(), value} {}

    ReadRequest()                   = delete;
    ReadRequest(const ReadRequest&) = delete;

    ~ReadRequest();

    void read();

    void checksum();

    void decompress();

    void decode();

    void wait();

    void checksum(bool);

private:

    ReadRequest(const std::string& URI, Decoder* decoder);
    ReadRequest(Stream, size_t offset, const std::string& key, Decoder*);

    Stream stream_;
    size_t offset_;
    std::string key_;
    std::string uri_;
    std::unique_ptr<Decoder> decoder_;
    std::unique_ptr<RecordItem> item_;
    bool do_checksum_{true};
    bool finished_{false};
};

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
