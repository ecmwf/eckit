/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/utils/LZ4Compressor.h"

#include <limits>

#include "lz4.h"  // header includes extern c linkage

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

LZ4Compressor::LZ4Compressor() {}

LZ4Compressor::~LZ4Compressor() {}

size_t LZ4Compressor::compress(const void* in, size_t len, Buffer& out) const {
    ASSERT(len <= std::numeric_limits<int>::max());
    ASSERT(out.size() <= std::numeric_limits<int>::max());

    const int maxcompressed = LZ4_compressBound(int(len));

    if (int(out.size()) < maxcompressed) {
        out.resize(size_t(maxcompressed));
    }

    const int compressed = LZ4_compress_default(static_cast<const char*>(in), out, int(len), maxcompressed);

    if (compressed <= 0) {
        std::ostringstream msg;
        msg << "returned " << compressed;
        throw FailedLibraryCall("LZ4", "LZ4_compress_default", msg.str(), Here());
    }

    return size_t(compressed);
}

void LZ4Compressor::uncompress(const void* in, size_t len, Buffer& out, size_t outlen) const {

    if (out.size() < outlen) {
        out.resize(outlen);
    }

    ASSERT(len <= std::numeric_limits<int>::max());
    ASSERT(out.size() <= std::numeric_limits<int>::max());

    const auto uncompressed = LZ4_decompress_safe(static_cast<const char*>(in), out, int(len), int(out.size()));

    if (uncompressed < 0) {
        std::ostringstream msg;
        msg << "returned " << uncompressed;
        throw FailedLibraryCall("LZ4", "LZ4_decompress_safe", msg.str(), Here());
    }

    ASSERT(uncompressed == outlen);
}

CompressorBuilder<LZ4Compressor> lz4("lz4");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
