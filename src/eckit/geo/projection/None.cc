// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/projection/None.h"


namespace eckit::geo::projection {


static ProjectionRegisterType<None> PROJECTION("none");


const std::string& None::type() const {
    static const std::string type{"none"};
    return type;
}


}  // namespace eckit::geo::projection
