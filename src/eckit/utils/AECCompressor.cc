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

size_t AECCompressor::minInputSize(const size_t inputSize, const aec_stream &strm) {
    int blockSizeBytes = strm.bits_per_sample * strm.block_size / 8;
    int minSize = (inputSize/blockSizeBytes);
    if (inputSize % blockSizeBytes > 0)
        minSize++;

    return minSize * blockSizeBytes;
}

size_t AECCompressor::compress(const eckit::Buffer& inTmp, ResizableBuffer& out) const{
    std::ostringstream msg;

    struct aec_stream strm;

    /* input data is 16 bits wide */
    strm.bits_per_sample = 16;
    /* define a block size of 64 */
    strm.block_size = 64;
    /* the reference sample interval is set to 128 blocks */
    strm.rsi = 128;
    /* input data is signed and needs to be preprocessed */
    strm.flags = AEC_DATA_PREPROCESS | AEC_DATA_MSB;

    Buffer in(inTmp, minInputSize(inTmp.size(), strm));

    unsigned int maxcompressed = (size_t) (1.2*in.size());
    if(out.size() < maxcompressed)
        out.resize(maxcompressed);
    unsigned int bufferSize = out.size();

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
    size_t outSize = strm.total_out;

    /* free all resources used by encoder */
    ret = aec_encode_end(&strm);
    if (ret == AEC_OK)
        return outSize;

    msg << "returned " << ret;
    throw FailedLibraryCall("AEC", "aec_encode_end", msg.str(), Here());
}

size_t AECCompressor::uncompress(const eckit::Buffer& in, ResizableBuffer& out) const {
    std::ostringstream msg;

    // AEC assumes you have transmitted the original size separately
    // We assume here that out is correctly sized

    struct aec_stream strm;

    /* input data is 16 bits wide */
    strm.bits_per_sample = 16;
    /* define a block size of 64 */
    strm.block_size = 64;
    /* the reference sample interval is set to 128 blocks */
    strm.rsi = 128;
    /* input data is signed and needs to be preprocessed */
    strm.flags = AEC_DATA_PREPROCESS | AEC_DATA_MSB;

    /* pointer to input */
    strm.next_in = (unsigned char *)in.data();
    /* length of input in bytes */
    strm.avail_in = in.size();

    Buffer outTmp(minInputSize(out.size(), strm));

    /* pointer to output buffer */
    strm.next_out = (unsigned char *)outTmp.data();
    /* length of output buffer in bytes */
    strm.avail_out = outTmp.size();

    /* initialize encoding */
    int ret = aec_decode_init(&strm);
    if (ret != AEC_OK) {
        msg << "returned " << ret;
        throw FailedLibraryCall("AEC", "aec_decode_init", msg.str(), Here());
    }

    ret = aec_decode(&strm, AEC_FLUSH);
    if (ret!= AEC_OK) {
        msg << "returned " << ret;
        throw FailedLibraryCall("AEC", "aec_decode", msg.str(), Here());
    }
    size_t outSize = strm.total_out;

    /* free all resources used by encoder */
    ret = aec_decode_end(&strm);
    if (ret == AEC_OK) {
        ::memcpy(out, outTmp, out.size());
        return out.size();
    }
    msg << "returned " << ret;
    throw FailedLibraryCall("AEC", "aec_decode_end", msg.str(), Here());
}

CompressorBuilder<AECCompressor> aec("aec");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
