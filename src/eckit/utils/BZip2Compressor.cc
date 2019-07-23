/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "bzlib.h"
#include "eckit/utils/BZip2Compressor.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/ResizableBuffer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

BZip2Compressor::BZip2Compressor() {
}

BZip2Compressor::~BZip2Compressor() {
}

size_t BZip2Compressor::compress(const eckit::Buffer& in, ResizableBuffer& out) const{
    std::ostringstream msg;

    unsigned int maxcompressed = (size_t) (1.01*in.size() + 600);
    if(out.size() < maxcompressed)
        out.resize(maxcompressed);
    unsigned int bufferSize = out.size();

    bz_stream strm;
    strm.avail_in = 0UL;
    strm.next_in = NULL;
    strm.next_out = NULL;
    strm.bzalloc = NULL;
    strm.bzfree = NULL;
    strm.opaque = NULL;

    int ret = BZ2_bzCompressInit(&strm, 9, 0, 30);
    if (ret != BZ_OK) {
        msg << "returned " << ret;
        throw FailedLibraryCall("BZlib2", "BZ2_bzCompressInit", msg.str(), Here());
    }

    strm.next_in = (char *)in.data();
    strm.avail_in = in.size();
    strm.next_out = out.data();
    strm.avail_out = bufferSize;

    ret = BZ2_bzCompress(&strm, BZ_FINISH);
    if (ret != BZ_STREAM_END && ret != BZ_OK) {
        msg << "returned " << ret;
        throw FailedLibraryCall("BZlib2", "BZ2_bzCompress", msg.str(), Here());
    }

    size_t outSize = bufferSize - strm.avail_out;

    strm.avail_in = 0;
    strm.next_in = NULL;

    ret = BZ2_bzCompressEnd(&strm);
    if (ret == BZ_OK)
        return outSize;

    msg << "returned " << ret;
    throw FailedLibraryCall("BZlib2", "BZ2_bzCompressEnd", msg.str(), Here());
}

size_t BZip2Compressor::uncompress(const eckit::Buffer& in, ResizableBuffer& out) const {
    std::ostringstream msg;

    // BZip2 assumes you have transmitted the original size separately
    // We assume here that out is correctly sized

    bz_stream strm;
    strm.avail_in = 0UL;
    strm.next_in = NULL;
    strm.next_out = NULL;
    strm.bzalloc = NULL;
    strm.bzfree = NULL;
    strm.opaque = NULL;

    int ret = BZ2_bzDecompressInit(&strm, 0, 0);
    if (ret != BZ_OK) {
        msg << "returned " << ret;
        throw FailedLibraryCall("BZlib2", "BZ2_bzDecompressInit", msg.str(), Here());
    }

    strm.next_in = (char *)in.data();
    strm.avail_in = in.size();
    strm.next_out = out.data();
    strm.avail_out = out.size();
    unsigned int bufferSize = out.size();

    ret = BZ2_bzDecompress(&strm);
    if (ret != BZ_STREAM_END && ret != BZ_OK) {
        msg << "returned " << ret;
        throw FailedLibraryCall("BZlib2", "BZ2_bzDecompress", msg.str(), Here());
    }

    size_t outSize = bufferSize - strm.avail_out;

    strm.next_out = NULL;
    ret = BZ2_bzDecompressEnd(&strm);
    if (ret == BZ_OK)
        return outSize;

    msg << "returned " << ret;
    throw FailedLibraryCall("BZlib2", "BZ2_bzDecompressEnd", msg.str(), Here());
}

CompressorBuilder<BZip2Compressor> bzip2("bzip2");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
