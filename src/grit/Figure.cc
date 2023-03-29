/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "grit/Figure.h"

#include <cassert>

#include "grit/util.h"


namespace grit {


Figure::Figure(double R) : Figure(R, R) {}


Figure::Figure(double a, double b) :
    a_(a), b_(b), R_(util::approximately_equal(a, b) ? a : std::numeric_limits<double>::signaling_NaN()) {
    assert(a > 0. && "Figure::a > 0.");
    assert(b > 0. && "Figure::b > 0.");
}


}  // namespace grit
