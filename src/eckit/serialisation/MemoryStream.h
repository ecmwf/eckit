// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
