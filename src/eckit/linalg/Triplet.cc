// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/linalg/Triplet.h"

#include <ostream>

namespace eckit::linalg {

//----------------------------------------------------------------------------------------------------------------------

void Triplet::print(std::ostream& os) const {
    os << "Triplet["
       << "row=" << row_ << ","
       << "col=" << col_ << ","
       << "val=" << val_ << "]";
}

std::ostream& operator<<(std::ostream& os, const eckit::linalg::Triplet& p) {
    p.print(os);
    return os;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::linalg
