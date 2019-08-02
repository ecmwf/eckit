/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// #include "snappy.h"
#include "snappy-c.h"

#include "eckit/utils/SnappyCompressor.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/ResizableBuffer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

SnappyCompressor::SnappyCompressor() {}

SnappyCompressor::~SnappyCompressor() {}

size_t SnappyCompressor::compress(const eckit::Buffer& in, ResizableBuffer& out) const {

    size_t maxcompressed = snappy_max_compressed_length(in.size());
    if (out.size() < maxcompressed)
        out.resize(maxcompressed);

    size_t len           = out.size();
    snappy_status status = snappy_compress(in, in.size(), out, &len);

    if (status == SNAPPY_OK)
        return len;

    std::ostringstream msg;
    if (status == SNAPPY_INVALID_INPUT)
        msg << "invalid input to compress";
    if (status == SNAPPY_BUFFER_TOO_SMALL)
        msg << "output buffer too small, size " << out.size();

    throw FailedLibraryCall("snappy", "compress", msg.str(), Here());
}

size_t SnappyCompressor::uncompress(const eckit::Buffer& in, ResizableBuffer& out) const {
    snappy_status status;

    size_t uncompressed;
    status = snappy_uncompressed_length(in, in.size(), &uncompressed);
    if (status != SNAPPY_OK) {
        throw FailedLibraryCall("snappy", "snappy_uncompressed_lengths", "returned != SNAPPY_OK", Here());
    }

    if (out.size() < uncompressed)
        out.resize(uncompressed);

    status = snappy_uncompress(in, in.size(), out, &uncompressed);

    if (status == SNAPPY_OK)
        return uncompressed;

    std::ostringstream msg;
    if (status == SNAPPY_INVALID_INPUT)
        msg << "invalid input to compress";
    if (status == SNAPPY_BUFFER_TOO_SMALL)
        msg << "output buffer too small, size " << out.size();

    throw FailedLibraryCall("snappy", "compress", msg.str(), Here());
}

CompressorBuilder<SnappyCompressor> snappy("snappy");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
