// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/figure/Sun.h"

#include "eckit/geo/Figure.h"


namespace eckit::geo::figure {


static const FigureRegisterType<Sun> REGISTER("sun");


const Sun SUN;


}  // namespace eckit::geo::figure
