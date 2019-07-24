/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "libaec.h"
#include "eckit/utils/AECCompressor.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/ResizableBuffer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

AECCompressor::AECCompressor() {
}

AECCompressor::~AECCompressor() {
}

size_t AECCompressor::compress(const eckit::Buffer& in, ResizableBuffer& out) const{
    std::ostringstream msg;

    unsigned int maxcompressed = (size_t) (1.2*in.size());
    if(out.size() < maxcompressed)
        out.resize(maxcompressed);
    unsigned int bufferSize = out.size();


    struct aec_stream strm;

    /* input data is 32 bits wide */
    strm.bits_per_sample = 32;
    /* define a block size of 16 */
    strm.block_size = 16;
    /* the reference sample interval is set to 128 blocks */
    strm.rsi = 128;
    /* input data is signed and needs to be preprocessed */
    strm.flags = AEC_DATA_SIGNED | AEC_DATA_PREPROCESS;

    /* pointer to input */
    strm.next_in = (unsigned char *)in.data();
    /* length of input in bytes */
    strm.avail_in = in.size();

    /* pointer to output buffer */
    strm.next_out = (unsigned char *)out.data();

    /* length of output buffer in bytes */
    strm.avail_out = bufferSize;

    /* initialize encoding */
    int ret = aec_encode_init(&strm);
    if (ret != AEC_OK) {
        msg << "returned " << ret;
        throw FailedLibraryCall("AEC", "aec_encode_init", msg.str(), Here());
    }

    /* Perform encoding in one call and flush output. */
    /* In this example you must be sure that the output */
    /* buffer is large enough for all compressed output */
    ret = aec_encode(&strm, AEC_FLUSH);
    if (ret!= AEC_OK) {
        msg << "returned " << ret;
        throw FailedLibraryCall("AEC", "aec_encode", msg.str(), Here());
    }
    size_t outSize = bufferSize - strm.avail_out;

    /* free all resources used by encoder */
    ret = aec_encode_end(&strm);
    if (ret == AEC_OK)
        return outSize;





/*


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
        return outSize;*/

    msg << "returned " << ret;
    throw FailedLibraryCall("AEC", "aec_encode_end", msg.str(), Here());
}

size_t AECCompressor::uncompress(const eckit::Buffer& in, ResizableBuffer& out) const {
    std::ostringstream msg;

    // AEC assumes you have transmitted the original size separately
    // We assume here that out is correctly sized
/*
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
        throw FailedLibraryCall("AEX", "aec_encode_end", msg.str(), Here());
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
    throw FailedLibraryCall("BZlib2", "BZ2_bzDecompressEnd", msg.str(), Here());*/
}

CompressorBuilder<AECCompressor> aec("aec");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
