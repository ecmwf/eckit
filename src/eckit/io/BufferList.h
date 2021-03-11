/*
 * (C) Copyright 2021- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @date   March 2021

#pragma once

#include "eckit/memory/OnlyMovable.h"
#include "eckit/io/Length.h"
#include "eckit/io/Buffer.h"

#include <list>


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// A class to aggregate buffers into a single object that can be read as a whole


class BufferList : public OnlyMovable {

public: // methods

    void append(Buffer&& buf);

    Length size() const;

    Buffer consolidate() const;

private: // members

    std::list<Buffer> buffers_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
