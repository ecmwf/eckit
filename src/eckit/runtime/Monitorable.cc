// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/runtime/Monitorable.h"

#include "eckit/log/JSON.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Monitorable::Monitorable() : ClassExtent<Monitorable>(this) {}

Monitorable::~Monitorable() {}

void Monitorable::status(std::ostream& s) const {
    print(s);
    s << std::endl;
}

void Monitorable::print(std::ostream& s) const {
    s << "No print method" << std::endl;
}

void Monitorable::allStatuses(std::ostream& s) {
    callAll(&Monitorable::status, s);
}

void Monitorable::allJSON(JSON& s) {
    s.startList();
    callAll(&Monitorable::json, s);
    s.endList();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
