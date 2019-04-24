/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
