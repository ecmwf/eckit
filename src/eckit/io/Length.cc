// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "eckit/io/Length.h"
#include "eckit/persist/DumpLoad.h"
#include "eckit/serialisation/Stream.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& s, const Length& x) {
    return s << x.value_;
}

Stream& operator<<(Stream& s, const Length& x) {
    return s << x.value_;
}

Stream& operator>>(Stream& s, Length& x) {
    return s >> x.value_;
}

void Length::dump(DumpLoad& a) const {
    a.dump(value_);
}

void Length::load(DumpLoad& a) {
    a.load(value_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
