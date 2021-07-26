/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/utils/BZip2Compressor.h"

#include <limits>

#include "bzlib.h"  // header includes extern c linkage

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

inline void BZip2Call(int code, const char* bzip2_func, const eckit::CodeLocation& loc) {
    if (code < 0) {
        std::ostringstream msg;
        msg << "returned " << code;

        switch (code) {
            case BZ_SEQUENCE_ERROR:
                msg << " (BZ_SEQUENCE_ERROR)";
                break;
            case BZ_PARAM_ERROR:
                msg << " (BZ_PARAM_ERROR)";
                break;
            case BZ_MEM_ERROR:
                msg << " (BZ_MEM_ERROR)";
                break;
            case BZ_DATA_ERROR:
                msg << " (BZ_DATA_ERROR)";
                break;
            case BZ_DATA_ERROR_MAGIC:
                msg << " (BZ_DATA_ERROR_MAGIC)";
                break;
            case BZ_IO_ERROR:
                msg << " (BZ_IO_ERROR)";
                break;
            case BZ_UNEXPECTED_EOF:
                msg << " (BZ_UNEXPECTED_EOF)";
                break;
            case BZ_OUTBUFF_FULL:
                msg << " (BZ_OUTBUFF_FULL)";
                break;
            case BZ_CONFIG_ERROR:
                msg << " (BZ_CONFIG_ERROR)";
                break;
            default:
                msg << " (UNRECOGNIZED ERROR)";
        }
        throw FailedLibraryCall("BZlib2", bzip2_func, msg.str(), loc);
    }
}

#define BZ2_CALL(a) BZip2Call(a, #a, Here())

//----------------------------------------------------------------------------------------------------------------------

BZip2Compressor::BZip2Compressor() {}

BZip2Compressor::~BZip2Compressor() {}

size_t BZip2Compressor::compress(const void* in, size_t len, Buffer& out) const {
    std::ostringstream msg;

    // https://sourceware.org/bzip2/manual/manual.html#bzbufftobuffcompress
    // To guarantee that the compressed data will fit in its buffer, allocate an output buffer of size 1% larger than
    // the uncompressed data, plus six hundred extra bytes.
    size_t maxcompressed = (size_t)(1.01 * len + 600);
    if (out.size() < maxcompressed)
        out.resize(maxcompressed);
    size_t bufferSize = out.size();

    ASSERT(len < std::numeric_limits<int>::max());
    ASSERT(maxcompressed < std::numeric_limits<int>::max());
    ASSERT(bufferSize < std::numeric_limits<int>::max());

    bz_stream strm;
    strm.avail_in = 0UL;
    strm.next_in  = nullptr;
    strm.next_out = nullptr;
    strm.bzalloc  = nullptr;
    strm.bzfree   = nullptr;
    strm.opaque   = nullptr;

    /// @note see https://sourceware.org/bzip2/manual/manual.tml#bzcompress-init
    BZ2_CALL(BZ2_bzCompressInit(&strm, 9, 0, 30));

    strm.next_in   = (char*)in;
    strm.avail_in  = len;
    strm.next_out  = (char*)out.data();
    strm.avail_out = bufferSize;

    BZ2_CALL(BZ2_bzCompress(&strm, BZ_FINISH));

    size_t outSize = bufferSize - strm.avail_out;

    strm.avail_in = 0;
    strm.next_in  = nullptr;

    BZ2_CALL(BZ2_bzCompressEnd(&strm));

    return outSize;
}

void BZip2Compressor::uncompress(const void* in, size_t len, Buffer& out, size_t outlen) const {
    ASSERT(len < std::numeric_limits<int>::max());

    if (out.size() < outlen) {
        out.resize(outlen);
    }

    bz_stream strm;
    strm.avail_in = 0UL;
    strm.next_in  = nullptr;
    strm.next_out = nullptr;
    strm.bzalloc  = nullptr;
    strm.bzfree   = nullptr;
    strm.opaque   = nullptr;

    BZ2_CALL(BZ2_bzDecompressInit(&strm, 0, 0));

    strm.next_in            = (char*)in;
    strm.avail_in           = len;
    strm.next_out           = (char*)out.data();
    strm.avail_out          = outlen;
    unsigned int bufferSize = outlen;

    BZ2_CALL(BZ2_bzDecompress(&strm));

    size_t outSize = bufferSize - strm.avail_out;

    ASSERT(outSize == outlen);

    strm.next_out = nullptr;
    BZ2_CALL(BZ2_bzDecompressEnd(&strm));
}

CompressorBuilder<BZip2Compressor> bzip2("bzip2");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
