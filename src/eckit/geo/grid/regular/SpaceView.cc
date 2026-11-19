// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/grid/regular/SpaceView.h"

#include <cmath>
#include <memory>
#include <string>
#include <utility>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/geo/Projection.h"
#include "eckit/geo/order/RegularScan.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::grid::regular {


struct SpaceView::Geometry {
    RegularXY::RangeXY x;
    RegularXY::RangeXY y;
    PointXY false_xy;
    std::unique_ptr<const Projection> projection;
};


SpaceView::Geometry SpaceView::make_geometry(const Parameters& p, const Spec& spec) {
    const std::unique_ptr<const Figure> figure(FigureFactory::build(spec));
    const order::RegularScan scan(spec);

    // the satellite height above the surface, and the scanning angle subtended by the Earth (on the projection plane)
    const auto height = (p.nr - 1.) * figure->a();
    const auto angle  = 2. * std::asin(1. / p.nr) * height;

    const auto dx = (scan.is_scan_i_positive() ? angle : -angle) / p.dx;
    const auto dy = (scan.is_scan_j_positive() ? angle : -angle) * figure->b() / figure->a() / p.dy;

    std::unique_ptr<spec::Custom> projection(
        new spec::Custom{{"type", "space_view"}, {"h", height}, {"lon_0", p.lon_0}, {"reverse", true}});
    figure->fill_projection_spec(*projection);

    // the false easting/northing places the first grid point at (0, 0)
    return {{0., static_cast<double>(p.nx - 1) * dx, dx},
            {0., static_cast<double>(p.ny - 1) * dy, dy},
            {(p.xp - p.xo) * dx, (p.yp - p.yo) * dy},
            std::unique_ptr<const Projection>(ProjectionFactory::build(*projection))};
}


SpaceView::Parameters SpaceView::Parameters::make_from_spec(const Spec& spec) {
    auto positive = [&spec](const std::string& key) {
        if (double value = 0.; spec.get(key, value) && types::is_strictly_greater(value, 0.)) {
            return value;
        }
        throw exception::SpecError("SpaceView: '" + key + "' expected (greater than zero)", Here());
    };

    Parameters p{static_cast<size_t>(positive("nx")),
                 static_cast<size_t>(positive("ny")),
                 positive("dx"),
                 positive("dy"),
                 spec.get_double("xp", 0.),
                 spec.get_double("yp", 0.),
                 spec.get_double("xo", 0.),
                 spec.get_double("yo", 0.),
                 positive("nr"),
                 spec.get_double("lon_0", 0.)};

    if (!types::is_strictly_greater(p.nr, 1.)) {
        throw exception::SpecError("SpaceView: 'nr' expected greater than one (the satellite outside the Earth)",
                                   Here());
    }

    return p;
}


SpaceView::SpaceView(const Spec& spec) : SpaceView(Parameters::make_from_spec(spec), spec) {}


SpaceView::SpaceView(const Parameters& p, const Spec& spec) : SpaceView(p, spec, make_geometry(p, spec)) {}


SpaceView::SpaceView(const Parameters& p, const Spec& spec, Geometry&& g) :
    RegularXY(g.x, g.y, std::move(g.projection), g.false_xy), parameters_(p) {
    // the order can scan j first, or alternate rows
    if (spec.has("order")) {
        scan(order::RegularScan(spec));
    }
}


const std::string& SpaceView::type() const {
    static const std::string type{"space_view"};
    return type;
}


void SpaceView::fill_spec(spec::Custom& custom) const {
    const auto& p = parameters_;

    custom.set("type", type());
    custom.set("nx", p.nx);
    custom.set("ny", p.ny);
    custom.set("dx", p.dx);
    custom.set("dy", p.dy);
    custom.set("xp", p.xp);
    custom.set("yp", p.yp);

    if (p.xo != 0. || p.yo != 0.) {
        custom.set("xo", p.xo);
        custom.set("yo", p.yo);
    }

    custom.set("nr", p.nr);

    if (p.lon_0 != 0.) {
        custom.set("lon_0", p.lon_0);
    }

    projection().figure().fill_projection_spec(custom);

    if (scan().order() != order::RegularScan::order_default()) {
        custom.set("order", scan().order());
    }
}


static const GridRegisterType<SpaceView> GRIDTYPE("space_view");


}  // namespace eckit::geo::grid::regular
