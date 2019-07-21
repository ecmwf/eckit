/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/utils/BZip2Compressor.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

BZip2Compressor::BZip2Compressor() {
}

BZip2Compressor::~BZip2Compressor() {
}

size_t BZip2Compressor::compress(const eckit::Buffer& in, ResizableBuffer& out) const{
    NOTIMP;
}
size_t BZip2Compressor::uncompress(const eckit::Buffer& in, ResizableBuffer& out) const {
    NOTIMP;
}

CompressorBuilder<BZip2Compressor> bzip2("bzip2");

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
