/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Manuel Fuentes
/// @author Tiago Quintino
/// @date   Apr 16

#ifndef eckit_serialisation_MemoryStream_H
#define eckit_serialisation_MemoryStream_H

#include "eckit/serialisation/Stream.h"

namespace eckit {

class Buffer;

//----------------------------------------------------------------------------------------------------------------------

class MemoryStream : public Stream {
public:

    MemoryStream(const Buffer&);
    MemoryStream(Buffer&);

    MemoryStream(const void* address, size_t size);
    MemoryStream(void* address, size_t size);

    ~MemoryStream();

    long read(void*, long) override;
    long write(const void*, long) override;
    void rewind() override;
    std::string name() const override;

    size_t position() const;

private:  // members

    char* address_;
    const size_t size_;

    size_t position_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit


#endif
