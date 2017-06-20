/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date June 2017

#ifndef eckit_CircularBuffer_h
#define eckit_CircularBuffer_h

#include "eckit/eckit.h"

#include "eckit/memory/NonCopyable.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

// A simple class to implement buffers

class CircularBuffer : public eckit::NonCopyable {

public: // methods

    CircularBuffer(size_t size = 64 * 1024);
    ~CircularBuffer();


    size_t write(const void* buffer, size_t length);
    size_t read(void* buffer, size_t length);

    size_t length() const;
    void clear();


private: // members

    char*  buffer_;
    size_t increment_;
    size_t size_;

    size_t pos_;
    size_t used_;


};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
