// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/util.h"


namespace eckit::geo::util {


template <>
pl_type pl_convert(const pl_type& pl) {
    return pl;
}


}  // namespace eckit::geo::util
