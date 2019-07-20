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
#include "eckit/io/ResizableBuffer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

LZ4Compressor::LZ4Compressor() {
}

LZ4Compressor::~LZ4Compressor() {
}

size_t LZ4Compressor::compress(const eckit::Buffer& in, ResizableBuffer& out) const{

    size_t maxcompressed = LZ4_compressBound(in.size());
    if(out.size() < maxcompressed)
        out.resize(maxcompressed);

    const int compressed = LZ4_compress_default(in, out, in.size(), maxcompressed);

    if (compressed > 0)
        return compressed;

    std::ostringstream msg;
    msg << "returned " << compressed;
    throw FailedLibraryCall("LZ4", "LZ4_compress_default", msg.str(), Here());
}

size_t LZ4Compressor::uncompress(const eckit::Buffer& in, ResizableBuffer& out) const {

    // LZ4 assumes you have transmitted the original size separately
    // We assume here that out is correctly sized

    const int uncompressed = LZ4_decompress_safe(in, out, in.size(), out.size());

    if (uncompressed >= 0)
        return uncompressed;

    std::ostringstream msg;
    msg << "returned " << uncompressed;
    throw FailedLibraryCall("LZa", "LZ4_decompress_safe", msg.str(), Here());
}

CompressorBuilder<LZ4Compressor> lz4("lz4");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
