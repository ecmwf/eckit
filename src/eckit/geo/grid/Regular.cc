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


#include "eckit/geo/grid/Regular.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Increments.h"
#include "eckit/geo/Spec.h"
#include "eckit/geo/etc/Grid.h"
#include "eckit/geo/iterator/Regular.h"
#include "eckit/geo/range/GaussianLatitude.h"
#include "eckit/geo/range/RegularCartesian.h"
#include "eckit/geo/range/RegularLatitude.h"
#include "eckit/geo/range/RegularLongitude.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/types/Fraction.h"
#include "eckit/utils/Translator.h"


namespace eckit::geo::grid {


double Regular::dx() const {
    return x().increment();
}


double Regular::dy() const {
    return y().increment();
}


Grid::iterator Regular::cbegin() const {
    return iterator{new geo::iterator::Regular(*this, 0)};
}


Grid::iterator Regular::cend() const {
    return iterator{new geo::iterator::Regular(*this, size())};
}


const Range& Regular::x() const {
    ASSERT(x_ && x_->size() > 0);
    return *x_;
}


const Range& Regular::y() const {
    ASSERT(y_ && y_->size() > 0);
    return *y_;
}


Regular::Regular(std::pair<Range*, Range*> xy, const area::BoundingBox& bbox) :
    Grid(bbox), x_(xy.first), y_(xy.second) {
    ASSERT(x_ && x_->size() > 0);
    ASSERT(y_ && y_->size() > 0);
}


std::pair<Range*, Range*> Regular::make_cartesian_ranges_from_spec(const Spec& spec) {
    Increments inc(spec);
    Shape shape(spec);

    // FIXME This is a hack, we should not be using these keys
    Point2 a{spec.get_double("longitudeOfFirstGridPointInDegrees"),
             spec.get_double("latitudeOfFirstGridPointInDegrees")};
    Point2 b{a.X + inc.dx * (shape.nx - 1), a.Y - inc.dy * (shape.ny - 1)};

    return {new range::RegularCartesian(shape.nx, a.X, b.X), new range::RegularCartesian(shape.ny, a.Y, b.Y)};
}


#define POSITIVE_REAL "[+]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][-+][0-9]+)?"
static const std::string REGULAR_LL_PATTERN("(" POSITIVE_REAL ")/(" POSITIVE_REAL ")");
#undef POSITIVE_REAL


RegularLL::RegularLL(const Spec& spec) :
    RegularLL(Increments{spec}, area::BoundingBox{spec}, [&spec]() -> PointLonLat {
        if (double lon = 0., lat = 0.; spec.get("reference_lon", lon) && spec.get("reference_lat", lat)) {
            return {lon, lat};
        }

        area::BoundingBox area{spec};
        return {area.west, area.south};
    }()) {
    ASSERT(size() > 0);
}


RegularLL::RegularLL(const Increments& inc, const area::BoundingBox& bbox) :
    RegularLL(inc, bbox, {bbox.south, bbox.west}) {}


RegularLL::RegularLL(const Increments& inc, const area::BoundingBox& bbox, const PointLonLat& ref) :
    Regular({new range::RegularLongitude(inc.dx, bbox.west, bbox.east, ref.lon, 0.),
             new range::RegularLatitude(inc.dy, bbox.north, bbox.south, ref.lat, 0.)},
            bbox) {
    ASSERT(size() > 0);
}


Spec* RegularLL::spec(const std::string& name) {
    std::smatch match;
    std::regex_match(name, match, std::regex(REGULAR_LL_PATTERN));
    ASSERT(match.size() == 9);

    auto d = Translator<std::string, double>{};

    return new spec::Custom({{"type", "regular_ll"}, {"grid", std::vector<double>{d(match[1]), d(match[5])}}});
}


void RegularLL::spec(spec::Custom& custom) const {
    custom.set("grid", std::vector<double>{dx(), dy()});
    boundingBox().spec(custom);
}


RegularGaussian::RegularGaussian(const Spec& spec) :
    RegularGaussian(spec.get_unsigned("N"), area::BoundingBox(spec)) {}


RegularGaussian::RegularGaussian(size_t N, const area::BoundingBox& bbox) :
    Regular({range::RegularLongitude(4 * N, 0., 360.).crop(bbox.west, bbox.east),
             range::GaussianLatitude(N, false).crop(bbox.north, bbox.south)},
            bbox),
    N_(N) {
    ASSERT(size() > 0);
}


Grid* RegularGaussian::make_grid_cropped(const area::BoundingBox& crop) const {
    if (auto bbox(boundingBox()); crop.intersects(bbox)) {
        return new RegularGaussian(N_, bbox);
    }

    throw Exception("RegularGaussian: cannot crop grid (empty intersection)");
}


Spec* RegularGaussian::spec(const std::string& name) {
    auto N = Translator<std::string, size_t>{}(name.substr(1));
    return new spec::Custom({{"type", "regular_gg"}, {"N", N}});
}


void RegularGaussian::spec(spec::Custom& custom) const {
    custom.set("grid", "F" + std::to_string(N_));
    boundingBox().spec(custom);
}


struct Mercator final : public Regular {
    explicit Mercator(const Spec& spec) :
        Regular(Regular::make_cartesian_ranges_from_spec(spec), area::BoundingBox{spec}) {}
    void spec(spec::Custom& custom) const override {
        custom.set("type", "mercator");
        custom.set("grid", std::vector<double>{dx(), dy()});
        custom.set("shape", std::vector<long>{static_cast<long>(nx()), static_cast<long>(ny())});

        // FIXME a lot more stuff to add here!
        //...

        if (std::string name; SpecByName::instance().match(custom, name)) {
            custom.clear();
            custom.set("grid", name);
        }
    }
};


struct LambertAzimuthalEqualArea final : public Regular {
    explicit LambertAzimuthalEqualArea(const Spec& spec) :
        Regular(Regular::make_cartesian_ranges_from_spec(spec), area::BoundingBox{spec}) {}
    void spec(spec::Custom& custom) const override {
        custom.set("type", "lambert_azimuthal_equal_area");
        custom.set("grid", std::vector<double>{dx(), dy()});
        custom.set("shape", std::vector<long>{static_cast<long>(nx()), static_cast<long>(ny())});

        // FIXME a lot more stuff to add here!
        //...

        if (std::string name; SpecByName::instance().match(custom, name)) {
            custom.clear();
            custom.set("grid", name);
        }
    }
};


static const GridRegisterName<RegularLL> __grid_pattern1(REGULAR_LL_PATTERN);
static const GridRegisterName<RegularGaussian> __grid_pattern2("[fF][1-9][0-9]*");

static const GridRegisterType<RegularLL> __grid_type1("regular_ll");
static const GridRegisterType<RegularGaussian> __grid_type2("regular_gg");
static const GridRegisterType<Mercator> __grid_type3("mercator");
static const GridRegisterType<LambertAzimuthalEqualArea> __grid_type4("lambert_azimuthal_equal_area");


}  // namespace eckit::geo::grid
