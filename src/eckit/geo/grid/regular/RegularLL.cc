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


#include "eckit/geo/grid/regular/RegularLL.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Increments.h"
#include "eckit/geo/iterator/Regular.h"
#include "eckit/geo/range/RegularLongitude.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util/regex.h"
#include "eckit/types/Fraction.h"
#include "eckit/utils/Translator.h"


namespace eckit::geo::grid::regular {


namespace {


struct DiscreteRange {
    DiscreteRange(double _a, double _b, double _inc, double _ref) :
        DiscreteRange(Fraction{_a}, Fraction{_b}, Fraction{_inc}, Fraction{_ref}) {}

    DiscreteRange(double _a, double _b, double _inc, double _ref, double period) :
        DiscreteRange(Fraction{_a}, Fraction{_b}, Fraction{_inc}, Fraction{_ref}, Fraction{period}) {}

    DiscreteRange(const Fraction& _a, const Fraction& _b, const Fraction& _inc, const Fraction& _ref) :
        inc(_inc), periodic(false) {
        ASSERT(_a <= _b);
        ASSERT(_inc >= 0);

        auto adjust = [](const Fraction& target, const Fraction& inc, bool up) -> Fraction {
            ASSERT(inc > 0);

            auto r = target / inc;
            auto n = r.integralPart();

            if (!r.integer() && (r > 0) == up) {
                n += (up ? 1 : -1);
            }

            return n * inc;
        };

        if (inc == 0) {
            b = a = _a;
            n     = 1;
            return;
        }

        auto shift = (_ref / inc).decimalPart() * _inc;
        a          = shift + adjust(_a - shift, inc, true);

        if (_b == _a) {
            b = a;
        }
        else {
            auto c = shift + adjust(_b - shift, inc, false);
            c      = a + ((c - a) / inc).integralPart() * inc;
            b      = c < a ? a : c;
        }

        n = static_cast<size_t>(((b - a) / inc).integralPart() + 1);

        ASSERT(a <= b);
        ASSERT(n >= 1);
    }

    DiscreteRange(
        const Fraction& _a, const Fraction& _b, const Fraction& _inc, const Fraction& _ref, const Fraction& period) :
        DiscreteRange(_a, _b, _inc, _ref) {
        ASSERT(period > 0);

        if ((n - 1) * inc >= period) {
            n -= 1;
            ASSERT(n * inc == period || (n - 1) * inc < period);

            b = a + (n - 1) * inc;
        }

        if (n * inc == period) {
            periodic = true;
            b        = a + n * inc;
        }
    }

    Fraction a;
    Fraction b;
    Fraction inc;
    size_t n;
    bool periodic;
};


PointLonLat make_reference_from_spec(const Spec& spec) {
    if (double lon = 0, lat = 0; spec.get("reference_lon", lon) && spec.get("reference_lat", lat)) {
        return {lon, lat};
    }

    area::BoundingBox area(spec);
    return {area.west, area.south};
}


}  // namespace


RegularLL::Internal::Internal(const Increments& _inc, const area::BoundingBox& _bbox, const PointLonLat& _ref) :
    inc(_inc), bbox(_bbox), first(_ref) {

    const DiscreteRange lon(bbox.west, bbox.east, inc.west_east, _ref.lon, 360);
    const DiscreteRange lat(bbox.south, bbox.north, inc.south_north, _ref.lat);

    ni = lon.n;
    nj = lat.n;
    ASSERT(ni > 0);
    ASSERT(nj > 0);

    bbox = {lat.b, lon.a, lat.a, lon.b};
}


RegularLL::RegularLL(const Spec& spec) :
    RegularLL(Increments{spec}, area::BoundingBox{spec}, make_reference_from_spec(spec)) {}


RegularLL::RegularLL(const Increments& inc, const area::BoundingBox& bbox) :
    RegularLL(inc, bbox, {bbox.south, bbox.west}) {}


RegularLL::RegularLL(const Increments& inc, const area::BoundingBox& bbox, const PointLonLat& ref) :
    RegularLL(Internal{inc, bbox, ref}) {}


RegularLL::RegularLL(Internal&& internal) :
    Regular(internal.bbox),
    internal_(internal),
    range_longitude_(new range::RegularLongitude(internal_.ni, internal_.bbox.east, internal_.bbox.west)),
    range_latitude_(new range::RegularLongitude(internal_.nj, internal_.bbox.north, internal_.bbox.south)) {
    ASSERT(size() > 0);
    ASSERT(ni() == range_longitude_->size());
    ASSERT(nj() == range_latitude_->size());
}


Grid::iterator RegularLL::cbegin() const {
    return iterator{new geo::iterator::Regular(*this, 0)};
}


Grid::iterator RegularLL::cend() const {
    return iterator{new geo::iterator::Regular(*this, size())};
}


const std::vector<double>& RegularLL::longitudes() const {
    return range_longitude_->values();
}


const std::vector<double>& RegularLL::latitudes() const {
    return range_latitude_->values();
}


#define POSITIVE_REAL "[+]?([0-9]*[.])?[0-9]+([eE][-+][0-9]+)?"


Spec* RegularLL::spec(const std::string& name) {
    static const std::string pattern("(" POSITIVE_REAL ")/(" POSITIVE_REAL ")");

    auto match = util::regex_match(pattern, name);
    ASSERT(match);
    ASSERT(match.size() == 7);  // because of sub-matches

    auto d = Translator<std::string, double>{};
    std::vector<double> increments{d(match[1]), d(match[4])};

    auto ni = 1;  // detail::RegularIterator(Fraction(0), Fraction(360), Fraction(increments[0]), Fraction(0),
                  // Fraction(360)).n();
    auto nj = 1;  // detail::RegularIterator(Fraction(-90), Fraction(90), Fraction(increments[1]), Fraction(0)).n();

    return new spec::Custom({{"type", "regular_ll"}, {"grid", increments}, {"ni", ni}, {"nj", nj}});
}


static const GridRegisterType<RegularLL> __grid_type("regular_ll");
static const GridRegisterName<RegularLL> __grid_pattern(POSITIVE_REAL "/" POSITIVE_REAL);


}  // namespace eckit::geo::grid::regular
