// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/utils/ZstdCompressor.h"

#include <sstream>

#include "zstd.h"

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ZstdCompressor::ZstdCompressor() : level_(Resource<int>("$ECKIT_ZSTD_LEVEL", 3)) {}

ZstdCompressor::~ZstdCompressor() {}

size_t ZstdCompressor::compress(const void* in, size_t len, Buffer& out) const {
    const size_t maxcompressed = ZSTD_compressBound(len);

    if (out.size() < maxcompressed) {
        out.resize(maxcompressed);
    }

    const size_t compressed = ZSTD_compress(out.data(), out.size(), in, len, level_);

    if (ZSTD_isError(compressed)) {
        throw FailedLibraryCall("zstd", "ZSTD_compress", ZSTD_getErrorName(compressed), Here());
    }

    return compressed;
}

void ZstdCompressor::uncompress(const void* in, size_t len, Buffer& out, size_t outlen) const {

    if (out.size() < outlen) {
        out.resize(outlen);
    }

    const size_t uncompressed = ZSTD_decompress(out.data(), out.size(), in, len);

    if (ZSTD_isError(uncompressed)) {
        throw FailedLibraryCall("zstd", "ZSTD_decompress", ZSTD_getErrorName(uncompressed), Here());
    }

    if (uncompressed != outlen) {
        std::ostringstream msg;
        msg << "decompressed " << uncompressed << " bytes, expected " << outlen;
        throw FailedLibraryCall("zstd", "ZSTD_decompress", msg.str(), Here());
    }
}

CompressorBuilder<ZstdCompressor> zstd("zstd");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
