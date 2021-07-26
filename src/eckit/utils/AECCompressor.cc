/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/utils/AECCompressor.h"

#include <cstring>

extern "C" {  // libaec.h has c linkage
#include "libaec.h"
}

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"

// compression parameters heve been tuned for maximum compression ratio with GRIB2 3D fields.
// Performance on GRIB 1 sfc fields is bad with any parameter values
// https://sourceware.org/bzip2/manual/manual.html#bzcompress-init
//
// bits_per_sample: range [1..32] Storage size from sample size. If a sample requires less bits than the storage size
// provides, then you have to make sure that unused bits are not set. Libaec does not check this for performance reasons
// and will produce undefined output if unused bits are set.
// 1 -  8 bits 	1 byte
// 9 - 16 bits 	2 bytes
// 17 - 24 bits 	3 bytes (only if AEC_DATA_3BYTE is set)
// 25 - 32 bits 	4 bytes (if AEC_DATA_3BYTE is set)
// 17 - 32 bits 	4 bytes (if AEC_DATA_3BYTE is not set)
//
// block_size: range [8..64] Smaller blocks allow the compression to adapt more rapidly to changing source
// statistics. Larger blocks create less overhead but can be less efficient if source statistics change across the
// block.
//
// rsi: sets the reference sample interval. A large RSI will improve performance and efficiency. It will also increase
// memory requirements since internal buffering is based on RSI size. A smaller RSI may be desirable in situations where
// each RSI will be packetized and possible error propagation has to be minimized.


#define AEC_bits_per_sample 16
#define AEC_block_size 64
#define AEC_rsi 129
#define AEC_flags AEC_DATA_PREPROCESS | AEC_DATA_MSB


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

inline void AECCall(int code, const char* aec_func, const eckit::CodeLocation& loc) {
    if (code != AEC_OK) {
        std::ostringstream msg;
        msg << "returned " << code;

        switch (code) {
            case AEC_CONF_ERROR:
                msg << " (AEC_CONF_ERROR)";
                break;
            case AEC_STREAM_ERROR:
                msg << " (AEC_STREAM_ERROR)";
                break;
            case AEC_DATA_ERROR:
                msg << " (AEC_DATA_ERROR)";
                break;
            case AEC_MEM_ERROR:
                msg << " (AEC_MEM_ERROR)";
                break;
            default:
                msg << " (UNRECOGNIZED ERROR)";
        }
        throw FailedLibraryCall("AEC", aec_func, msg.str(), loc);
    }
}

#define AEC_CALL(a) AECCall(a, #a, Here())

//----------------------------------------------------------------------------------------------------------------------

AECCompressor::AECCompressor() {}

AECCompressor::~AECCompressor() {}

static size_t minInputSize(const size_t inputSize, const aec_stream& strm) {
    size_t blockSizeBytes = strm.bits_per_sample * strm.block_size / 8;
    size_t minSize        = (inputSize / blockSizeBytes);
    if (inputSize % blockSizeBytes > 0)
        minSize++;

    return minSize * blockSizeBytes;
}

size_t AECCompressor::compress(const void* inTmp, size_t len, Buffer& out) const {

    struct aec_stream strm;

    strm.bits_per_sample = AEC_bits_per_sample;
    strm.block_size      = AEC_block_size;
    strm.rsi             = AEC_rsi;
    strm.flags           = AEC_flags;

    Buffer in(minInputSize(len, strm));
    in.copy(inTmp, len);

    if (in.size() > len) {
        ::memset(in + len, 0, in.size() - len);
    }

    size_t maxcompressed = size_t(1.2 * in.size());
    if (out.size() < maxcompressed) {
        out.resize(maxcompressed);
    }

    strm.next_in  = (unsigned char*)in.data();
    strm.avail_in = in.size();

    strm.next_out  = (unsigned char*)out.data();
    strm.avail_out = out.size();

    AEC_CALL(aec_encode_init(&strm));

    // Perform encoding in one call and flush output.
    // you must be sure that the output buffer is large enough for all compressed output
    AEC_CALL(aec_encode(&strm, AEC_FLUSH));

    size_t outSize = strm.total_out;

    // free all resources used by encoder
    AEC_CALL(aec_encode_end(&strm));

    return outSize;
}

void AECCompressor::uncompress(const void* in, size_t len, Buffer& out, size_t outlen) const {

    struct aec_stream strm;

    strm.bits_per_sample = AEC_bits_per_sample;
    strm.block_size      = AEC_block_size;
    strm.rsi             = AEC_rsi;
    strm.flags           = AEC_flags;

    strm.next_in  = (const unsigned char*)(in);
    strm.avail_in = len;

    size_t outSize = minInputSize(outlen, strm);

    // If out is sized large enough, we can use its extra capacity.
    // Otherwise we allocate a temporary outTmp and at end we move outTmp into out.

    Buffer outTmp;
    if (out.size() >= outSize) {
        strm.next_out = (unsigned char*)out.data();
    }
    else {
        outTmp.resize(outSize);
        strm.next_out = (unsigned char*)outTmp.data();
    }
    strm.avail_out = outSize;

    AEC_CALL(aec_decode_init(&strm));

    AEC_CALL(aec_decode(&strm, AEC_FLUSH));

    ASSERT(strm.total_out == outSize);

    AEC_CALL(aec_decode_end(&strm));

    if (outTmp.size()) {
        out = std::move(outTmp);
    }
}

CompressorBuilder<AECCompressor> aec("aec");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
