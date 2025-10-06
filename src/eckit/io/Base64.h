/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
