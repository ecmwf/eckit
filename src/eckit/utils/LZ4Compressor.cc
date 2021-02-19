/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "lz4.h"

#include "eckit/utils/LZ4Compressor.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

LZ4Compressor::LZ4Compressor() {}

LZ4Compressor::~LZ4Compressor() {}

size_t LZ4Compressor::compress(const void* in, size_t len, Buffer& out) const {

    size_t maxcompressed = LZ4_compressBound(len);
    if (out.size() < maxcompressed)
        out.resize(maxcompressed);

    const auto compressed = LZ4_compress_default(static_cast<const char*>(in), out, len, maxcompressed);

    if (compressed > 0)
        return compressed;

    std::ostringstream msg;
    msg << "returned " << compressed;
    throw FailedLibraryCall("LZ4", "LZ4_compress_default", msg.str(), Here());
}

size_t LZ4Compressor::uncompress(const void* in, size_t len, Buffer& out) const {

    // LZ4 assumes you have transmitted the original size separately
    // We assume here that out is correctly sized
    if( out.size() == 0 ) {
        throw UnexpectedState("LZ4 decompression expects sufficiently large allocated out buffer", Here());
    }

    const int uncompressed = LZ4_decompress_safe(static_cast<const char*>(in), out, len, out.size());

    if (uncompressed >= 0)
        return uncompressed;

    std::ostringstream msg;
    msg << "returned " << uncompressed;
    throw FailedLibraryCall("LZa", "LZ4_decompress_safe", msg.str(), Here());
}

CompressorBuilder<LZ4Compressor> lz4("lz4");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
