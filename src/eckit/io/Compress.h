// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_Compress_h
#define eckit_Compress_h

#include <cstddef>

namespace eckit {

class DataHandle;

//-----------------------------------------------------------------------------

class Compress {

public:  // methods

    Compress(size_t maxBits = 16);
    // --

    size_t decode(DataHandle& in, DataHandle& out);
    size_t encode(DataHandle& in, DataHandle& out);

private:

    const size_t maxBits_;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
