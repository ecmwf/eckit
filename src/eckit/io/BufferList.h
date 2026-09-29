// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Simon Smart
/// @author Tiago Quintino
/// @date   March 2021

#pragma once

#include <list>

#include "eckit/io/Buffer.h"
#include "eckit/io/Length.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// A class to aggregate buffers into a single object that can be read as a whole

class BufferList {

public:  // methods

    BufferList() = default;

    BufferList(const BufferList&)            = delete;
    BufferList& operator=(const BufferList&) = delete;
    BufferList(BufferList&&)                 = default;
    BufferList& operator=(BufferList&&)      = default;

    ~BufferList() = default;

    void append(Buffer&& buf);

    size_t count() const { return buffers_.size(); }
    Length size() const;

    /// @note After consolidation the internal list is cleared and memory is deallocated
    /// @post count() == 0 and size() == 0
    Buffer consolidate();

private:  // members

    std::list<Buffer> buffers_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
