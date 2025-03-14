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


#include "eckit/geo/area/Polygon.h"

#include <algorithm>
#include <memory>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/spec/Custom.h"


namespace eckit::geo::area {


static const AreaRegisterType<Polygon> AREATYPE("polygon");


Polygon::Polygon(const Spec& spec) : Polygon(*std::unique_ptr<Polygon>(make_from_spec(spec))) {}


bool Polygon::contains(const eckit::geo::PointLonLat& p) const {
    auto c = std::count_if(begin(), end(), [&p](const auto& poly) { return poly.contains(p); });
    return c % 2 != 0;
}


const std::string& Polygon::type() const {
    static const std::string type{"polygon"};
    return type;
}


void Polygon::fill_spec(spec::Custom& custom) const {
    custom.set("type", type());
    custom.set(type(), "NOTIMP");
}


Polygon* Polygon::make_from_spec(const Spec& spec) {
    NOTIMP;
}


}  // namespace eckit::geo::area
