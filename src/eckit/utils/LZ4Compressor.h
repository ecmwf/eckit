// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_utils_LZ4Compressor_H
#define eckit_utils_LZ4Compressor_H

#include "eckit/utils/Compressor.h"

namespace eckit {

class Buffer;

//----------------------------------------------------------------------------------------------------------------------

class LZ4Compressor : public eckit::Compressor {

public:  // methods

    LZ4Compressor();

    ~LZ4Compressor() override;

    size_t compress(const void* in, size_t len, eckit::Buffer& out) const override;
    void uncompress(const void* in, size_t len, eckit::Buffer& out, size_t outlen) const override;

protected:  // methods
};

//----------------------------------------------------------------------------------------------------------------------

}  // end namespace eckit

#endif
