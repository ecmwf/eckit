/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
