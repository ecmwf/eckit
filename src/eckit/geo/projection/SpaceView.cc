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


#include "eckit/geo/projection/SpaceView.h"

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/spec/Custom.h"


namespace eckit::geo::projection {


SpaceView::SpaceView(const Spec&) {
    NOTIMP;
}


PointXY SpaceView::fwd(const PointLonLat&) const {
    NOTIMP;
}


PointLonLat SpaceView::inv(const PointXY&) const {
    NOTIMP;
}


const std::string& SpaceView::type() const {
    static const std::string type{"space-view"};
    return type;
}


void SpaceView::fill_spec(spec::Custom& custom) const {
    ProjectionOnFigure::fill_spec(custom);

    custom.set("projection", "geos");  //?
    NOTIMP;
}


}  // namespace eckit::geo::projection
