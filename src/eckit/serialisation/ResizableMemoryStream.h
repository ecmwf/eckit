// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Tiago Quintino
/// @date   Feb 2019

#ifndef eckit_serialisation_ResizableMemoryStream_H
#define eckit_serialisation_ResizableMemoryStream_H

#include "eckit/serialisation/Stream.h"

namespace eckit {

class Buffer;

//----------------------------------------------------------------------------------------------------------------------

class ResizableMemoryStream : public Stream {
public:

    ResizableMemoryStream(Buffer&);

    ~ResizableMemoryStream();

    long read(void*, long) override;
    long write(const void*, long) override;
    void rewind() override;

    std::string name() const override;

    size_t position() const;

private:  // members

    Buffer& buffer_;

    size_t position_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit


#endif
