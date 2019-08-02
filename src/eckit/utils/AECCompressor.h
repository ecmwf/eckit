/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_utils_AECCompressor_H
#define eckit_utils_AECCompressor_H

#include "eckit/utils/Compressor.h"

namespace eckit {

class Buffer;
class ResizableBuffer;

//----------------------------------------------------------------------------------------------------------------------

class AECCompressor : public eckit::Compressor {

public:  // methods
    AECCompressor();

    virtual ~AECCompressor();

    virtual size_t compress(const eckit::Buffer& in, eckit::ResizableBuffer& out) const;
    virtual size_t uncompress(const eckit::Buffer& in, eckit::ResizableBuffer& out) const;

protected:  // methods
    static size_t minInputSize(const size_t inputSize, const aec_stream& strm);
};

//----------------------------------------------------------------------------------------------------------------------

}  // end namespace eckit

#endif
