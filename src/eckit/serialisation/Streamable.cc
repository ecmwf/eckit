/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/serialisation/Streamable.h"


//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ClassSpec Streamable::classSpec_ = {
    0,
    "Streamable",
};
Reanimator<Streamable> Streamable::reanimator_;

Streamable::Streamable(Stream&) {}

void Streamable::encode(Stream&) const {}

std::string Streamable::className() const {
    return reanimator().spec().name_;
}

Stream& operator<<(Stream& s, const Streamable& x) {
    s.startObject();
    s << x.className();
    x.encode(s);
    s.endObject();
    return s;
}

bool Streamable::sameClass(const Streamable& other) const {
    return &reanimator() == &other.reanimator();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
