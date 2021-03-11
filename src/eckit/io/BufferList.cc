/*
 * (C) Copyright 2021- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/BufferList.h"

#include "eckit/io/Offset.h"

#include <utility>
#include <numeric>


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

void BufferList::append(Buffer&& buf) {
    buffers_.emplace_back(std::move(buf));
}

Length BufferList::size() const {
    return std::accumulate(buffers_.begin(), buffers_.end(), Length(0),
                           [](const Length& lhs, const Buffer& rhs) { return lhs + Length(rhs.size()); });
}

Buffer BufferList::consolidate() const {
    Buffer result(size());

    Offset offset = 0;
    for (const auto& buffer : buffers_) {
        result.copy(buffer, buffer.size(), offset);
        offset += buffer.size();
    }

    return result;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
