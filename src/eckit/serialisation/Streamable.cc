// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/serialisation/Streamable.h"


//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ClassSpec Streamable::classSpec_ = {
    nullptr,
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
