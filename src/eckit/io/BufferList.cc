// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/io/BufferList.h"

#include <numeric>
#include <utility>

#include "eckit/io/Offset.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

void BufferList::append(Buffer&& buf) {
    buffers_.emplace_back(std::move(buf));
}

Length BufferList::size() const {
    return std::accumulate(buffers_.begin(), buffers_.end(), Length(0),
                           [](const Length& lhs, const Buffer& rhs) { return lhs + Length(rhs.size()); });
}

Buffer BufferList::consolidate() {
    const size_t nbuffs = count();

    if (nbuffs == 0) {
        return Buffer();
    }

    // optimize for count() = 1
    // we can do this optimisation because consolidate() clears the contents of the list
    if (nbuffs == 1) {
        Buffer b = std::move(buffers_.front());
        buffers_.clear();
        return b;
    }

    Buffer result(size());
    Offset offset = 0;
    for (const auto& buffer : buffers_) {
        result.copy(buffer, buffer.size(), offset);
        offset += buffer.size();
    }

    buffers_.clear();  // deallocate all buffers
    return result;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
