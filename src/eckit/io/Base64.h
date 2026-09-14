// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_Base64_h
#define eckit_Base64_h

#include <cstddef>

namespace eckit {

class DataHandle;

//-----------------------------------------------------------------------------

class Base64 {

public:  // methods

    Base64(bool url = false);

    size_t decode(DataHandle& in, DataHandle& out);
    size_t encode(DataHandle& in, DataHandle& out);

private:

    unsigned char encode_[256];
    unsigned char decode_[256];
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
