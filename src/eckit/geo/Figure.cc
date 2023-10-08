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


#include "eckit/geo/Figure.h"

#include "eckit/exception/Exceptions.h"


namespace eckit::geo {


double Figure::R() const {
    throw NotImplemented("Figure::R", Here());
}


double Figure::a() const {
    throw NotImplemented("Figure::a", Here());
}


double Figure::b() const {
    throw NotImplemented("Figure::b", Here());
}


double Figure::eccentricity() const {
    return std::sqrt(1. - (b() * b()) / (a() * a()));
}


}  // namespace eckit::geo
