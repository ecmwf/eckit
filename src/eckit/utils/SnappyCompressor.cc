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

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

SnappyCompressor::SnappyCompressor() {}

SnappyCompressor::~SnappyCompressor() {}

size_t SnappyCompressor::compress(const void* in, size_t len, Buffer& out) const {

    size_t maxcompressed = snappy_max_compressed_length(len);
    if (out.size() < maxcompressed)
        out.resize(maxcompressed);

    size_t outlen           = out.size();
    snappy_status status = snappy_compress(static_cast<const char*>(in), len, out, &outlen);

    if (status == SNAPPY_OK)
        return outlen;

    std::ostringstream msg;
    if (status == SNAPPY_INVALID_INPUT)
        msg << "invalid input to compress";
    if (status == SNAPPY_BUFFER_TOO_SMALL)
        msg << "output buffer too small, size " << out.size();

    throw FailedLibraryCall("snappy", "compress", msg.str(), Here());
}

size_t SnappyCompressor::uncompress(const void* in, size_t len, Buffer& out) const {
    snappy_status status;

    size_t uncompressed;
    status = snappy_uncompressed_length(static_cast<const char*>(in), len, &uncompressed);
    if (status != SNAPPY_OK) {
        throw FailedLibraryCall("snappy", "snappy_uncompressed_lengths", "returned != SNAPPY_OK", Here());
    }

    if (out.size() < uncompressed)
        out.resize(uncompressed);

    status = snappy_uncompress(static_cast<const char*>(in), len, out, &uncompressed);

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
