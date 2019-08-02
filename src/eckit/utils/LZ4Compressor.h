/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_utils_LZ4Compressor_H
#define eckit_utils_LZ4Compressor_H

#include "eckit/utils/Compressor.h"

namespace eckit {

class Buffer;
class ResizableBuffer;

//----------------------------------------------------------------------------------------------------------------------

class LZ4Compressor : public eckit::Compressor {

public:  // methods
    LZ4Compressor();

    virtual ~LZ4Compressor();

    virtual size_t compress(const eckit::Buffer& in, eckit::ResizableBuffer& out) const;
    virtual size_t uncompress(const eckit::Buffer& in, eckit::ResizableBuffer& out) const;

protected:  // methods
};

//----------------------------------------------------------------------------------------------------------------------

}  // end namespace eckit

#endif
