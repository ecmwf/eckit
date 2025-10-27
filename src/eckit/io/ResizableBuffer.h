/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_io_ResizableBuffer_h
#define eckit_io_ResizableBuffer_h

#warning \
    "Header eckit/io/ResizableBuffer.h and class eckit::ResizableBuffer is deprecated -- include eckit/io/Buffer.h and use eckit::Buffer"
#include "eckit/io/Buffer.h"

namespace eckit {

using ResizableBuffer = Buffer;

}  // namespace eckit

#endif
