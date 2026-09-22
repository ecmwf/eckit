// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/geo/area/None.h"

#include "eckit/spec/Custom.h"


namespace eckit::geo::area {


const std::string& None::type() const {
    static const std::string type{"none"};
    return type;
}


void None::fill_spec(spec::Custom& custom) const {
    custom.set("type", type());
}


}  // namespace eckit::geo::area
