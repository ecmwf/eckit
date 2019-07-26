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

    // https://sourceware.org/bzip2/manual/manual.html#bzbufftobuffcompress
    // To guarantee that the compressed data will fit in its buffer, allocate an output buffer of size 1% larger than the uncompressed data, plus six hundred extra bytes.
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

    // https://sourceware.org/bzip2/manual/manual.html#bzcompress-init
    // int BZ2_bzCompressInit ( bz_stream *strm,
    //                         int blockSize100k,   - block size used for compression   range [1..9]    block size used is 100000 x this figure. 9 gives the best compression but takes most memory.
    //                         int verbosity,       - verbosity                         range [0..4]    0 is silent, greater numbers give increasingly verbose monitoring/debugging output.
    //                         int workFactor );    - threshold, fallback sorting algo  range [0..250]  controls how the compression phase behaves when presented with worst case, highly repetitive, input data
    //                                                                                                  in case of repetitive data, the library switches to a fallback algorithm. 3X slower. more stable.
    //                                                                                                  Lower values reduce the effort the standard algorithm will expend before resorting to the fallback. default value of 30
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
