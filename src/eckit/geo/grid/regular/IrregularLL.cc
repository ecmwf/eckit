// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/grid/regular/IrregularLL.h"

#include <memory>
#include <string>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/order/RegularScan.h"
#include "eckit/spec/Custom.h"


namespace eckit::geo::grid::regular {


namespace {


std::vector<double> values_from_spec(const Grid::Spec& spec, const std::string& key) {
    if (std::vector<double> v; spec.get(key, v) && !v.empty()) {
        return v;
    }

    throw exception::SpecError("IrregularLL: {latitudes: [...], longitudes: [...]} expected (not empty)", Here());
}


/// As the directions of the longitudes and latitudes
order::RegularScan scan_from_directions(const std::vector<double>& longitudes, const std::vector<double>& latitudes) {
    auto sign = [](const std::vector<double>& values) { return range::Irregular(values).increasing() ? "+" : "-"; };
    return order::RegularScan(std::string("i") + sign(longitudes) + "j" + sign(latitudes));
}


}  // namespace


IrregularLL::IrregularLL(const Spec& spec) :
    IrregularLL(values_from_spec(spec, "longitudes"), values_from_spec(spec, "latitudes")) {
    // the order is as the directions of the longitudes and latitudes, and can scan j first
    if (spec.has("order")) {
        order::RegularScan s(spec);
        if (s.is_scan_i_positive() != scan().is_scan_i_positive() ||
            s.is_scan_j_positive() != scan().is_scan_j_positive()) {
            throw exception::SpecError("IrregularLL: 'order' and the directions of the longitudes/latitudes differ",
                                       Here());
        }
        scan(s);
    }
}


IrregularLL::IrregularLL(const std::vector<double>& longitudes, const std::vector<double>& latitudes) :
    Regular(scan_from_directions(longitudes, latitudes)), x_(longitudes), y_(latitudes) {
    if (y_.min() < -PointLonLat::RIGHT_ANGLE || PointLonLat::RIGHT_ANGLE < y_.max()) {
        throw exception::SpecError("IrregularLL: latitudes expected in [-90, 90]", Here());
    }
}


const std::string& IrregularLL::type() const {
    static const std::string type{"irregular_ll"};
    return type;
}


Grid::BoundingBox* IrregularLL::calculate_bbox() const {
    return new BoundingBox{y_.max(), x_.min(), y_.min(), x_.max()};
}


Regular* IrregularLL::make_regular_cropped(const BoundingBox& bbox) const {
    std::unique_ptr<const range::Irregular> x(x_.make_cropped_range(bbox.west(), bbox.east()));
    std::unique_ptr<const range::Irregular> y(y_.make_cropped_range(bbox.south(), bbox.north()));
    return new IrregularLL(x->values(), y->values());
}


void IrregularLL::fill_spec(spec::Custom& custom) const {
    custom.set("type", type());
    custom.set("latitudes", y_.values());
    custom.set("longitudes", x_.values());

    if (scan().order() != order::RegularScan::order_default()) {
        custom.set("order", scan().order());
    }
}


static const GridRegisterType<IrregularLL> GRIDTYPE("irregular_ll");


}  // namespace eckit::geo::grid::regular
