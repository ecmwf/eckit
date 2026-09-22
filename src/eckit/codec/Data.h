// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>

#include "eckit/io/Buffer.h"

namespace eckit::codec {

class Stream;

//---------------------------------------------------------------------------------------------------------------------

class Data {
public:

    Data() = default;
    Data(void*, size_t);

    Data(Data&&)            = default;
    Data& operator=(Data&&) = default;

    operator const void*() const { return data(); }
    const void* data() const { return buffer_.data(); }
    size_t size() const { return size_; }

    void assign(const Data& other);
    void assign(const void*, size_t);
    void clear();

    std::uint64_t write(Stream& out) const;
    std::uint64_t read(Stream& in, size_t size);
    void compress(const std::string& compression);
    void decompress(const std::string& compression, size_t uncompressed_size);
    std::string checksum(const std::string& algorithm = "") const;

private:

    Buffer buffer_;
    size_t size_{0};
};

//---------------------------------------------------------------------------------------------------------------------

void encode_data(const Data&, Data& out);

//---------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::codec
