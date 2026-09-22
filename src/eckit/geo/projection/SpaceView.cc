// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/geo/projection/SpaceView.h"

#include "eckit/geo/Exceptions.h"
#include "eckit/spec/Custom.h"


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
    static const std::string type{"space_view"};
    return type;
}


void SpaceView::fill_spec(spec::Custom& custom) const {
    Projection::fill_spec(custom);

    custom.set("type", "geos");  //?
    NOTIMP;
}


}  // namespace eckit::geo::projection
