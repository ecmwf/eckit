// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/exception/Exceptions.h"

#include "eckit/log/OStreamTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

OStreamTarget::OStreamTarget(std::ostream& out) : out_(out) {}

OStreamTarget::~OStreamTarget() {}

void OStreamTarget::write(const char* start, const char* end) {
    if (start >= end)
        return;
    out_.write(start, end - start);
}
void OStreamTarget::flush() {
    out_.flush();
}

void OStreamTarget::print(std::ostream& s) const {
    s << "OStreamTarget()";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
