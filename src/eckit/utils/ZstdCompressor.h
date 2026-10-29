// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_utils_ZstdCompressor_H
#define eckit_utils_ZstdCompressor_H

#include "eckit/utils/Compressor.h"

namespace eckit {

class Buffer;

//----------------------------------------------------------------------------------------------------------------------

class ZstdCompressor : public eckit::Compressor {

public:  // methods

    ZstdCompressor();

    ~ZstdCompressor() override;

    size_t compress(const void* in, size_t len, eckit::Buffer& out) const override;
    void uncompress(const void* in, size_t len, eckit::Buffer& out, size_t outlen) const override;

private:  // members

    int level_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // end namespace eckit

#endif
