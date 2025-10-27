/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_utils_BZip2Compressor_H
#define eckit_utils_BZip2Compressor_H

#include "eckit/utils/Compressor.h"

namespace eckit {

class Buffer;

//----------------------------------------------------------------------------------------------------------------------

class BZip2Compressor : public eckit::Compressor {

public:  // methods

    BZip2Compressor();

    ~BZip2Compressor() override;

    size_t compress(const void* in, size_t len, eckit::Buffer& out) const override;
    void uncompress(const void* in, size_t len, eckit::Buffer& out, size_t outlen) const override;

protected:  // methods
};

//----------------------------------------------------------------------------------------------------------------------

}  // end namespace eckit

#endif
