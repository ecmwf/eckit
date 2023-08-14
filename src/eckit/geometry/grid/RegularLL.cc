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


#include "eckit/geometry/grid/RegularLL.h"

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "eckit/config/MappedConfiguration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/geometry/Point.h"
#include "eckit/geometry/Projection.h"
#include "eckit/geometry/grid/RegularLL.h"
#include "eckit/geometry/util/regex.h"
#include "eckit/types/Fraction.h"
#include "eckit/utils/Translator.h"


namespace eckit::geometry::grid {


namespace detail {


class RegularIterator {
public:
    RegularIterator(const Fraction& a, const Fraction& b, const Fraction& inc, const Fraction& ref);
    RegularIterator(const Fraction& a, const Fraction& b, const Fraction& inc, const Fraction& ref, const Fraction& period);

    static Fraction adjust(const Fraction& target, const Fraction& inc, bool up) {
        ASSERT(inc > 0);

        auto r = target / inc;
        auto n = r.integralPart();

        if (!r.integer() && (r > 0) == up) {
            n += (up ? 1 : -1);
        }

        return (n * inc);
    }

    const Fraction& a() const { return a_; }
    const Fraction& b() const { return b_; }
    const Fraction& inc() const { return inc_; }
    size_t n() const { return n_; }

private:
    Fraction a_;
    Fraction b_;
    Fraction inc_;
    size_t n_;
};


RegularIterator::RegularIterator(const Fraction& a, const Fraction& b, const Fraction& inc,
                                 const Fraction& ref) :
    inc_(inc) {
    ASSERT(a <= b);
    ASSERT(inc >= 0);

    if (inc_ == 0) {

        b_ = a_ = a;
        n_      = 1;
    }
    else {

        auto shift = (ref / inc_).decimalPart() * inc;
        a_         = shift + adjust(a - shift, inc_, true);

        if (b == a) {
            b_ = a_;
        }
        else {

            auto c = shift + adjust(b - shift, inc_, false);
            c      = a_ + ((c - a_) / inc_).integralPart() * inc_;
            b_     = c < a_ ? a_ : c;
        }

        n_ = size_t(((b_ - a_) / inc_).integralPart() + 1);
    }
    ASSERT(a_ <= b_);
    ASSERT(n_ >= 1);
}


RegularIterator::RegularIterator(const Fraction& a, const Fraction& b, const Fraction& inc,
                                 const Fraction& ref, const Fraction& period) :
    RegularIterator(a, b, inc, ref) {
    ASSERT(period > 0);

    if ((n_ - 1) * inc_ >= period) {
        n_ -= 1;
        ASSERT(n_ * inc_ == period || (n_ - 1) * inc_ < period);

        b_ = a_ + (n_ - 1) * inc_;
    }
}


}  // namespace detail


RegularLL::RegularLL(const Configuration& config) :
    Grid(config), increments_(config), reference_(bbox().south(), bbox().west()), ni_(0), nj_(0) {
    bbox(correctBoundingBox(bbox(), ni_, nj_, increments_, reference_));
    ASSERT(ni_ != 0);
    ASSERT(nj_ != 0);

    // confirm Ni/Nj from config (input)
    size_t ni = 0;
    size_t nj = 0;
    ASSERT(config.get("ni", ni));
    ASSERT(config.get("nj", nj));

    ASSERT(ni == ni_);
    ASSERT(nj == nj_);
}


RegularLL::RegularLL(const Increments& increments, const area::BoundingBox& bb, const PointLonLat& reference) :
    Grid(bb), increments_(increments), reference_(reference), ni_(0), nj_(0) {
    bbox(correctBoundingBox(bbox(), ni_, nj_, increments_, reference_));
    ASSERT(ni_ != 0);
    ASSERT(nj_ != 0);
}


bool RegularLL::isPeriodicWestEast() const {
    // if range West-East is within one increment (or greater than) 360 degree
    return bbox().east() - bbox().west() + increments_.west_east >= GLOBE;
}


bool RegularLL::includesNorthPole() const {
    // if North latitude is within one increment from North Pole
    return bbox().north() + increments_.south_north > NORTH_POLE;
}


bool RegularLL::includesSouthPole() const {
    // if South latitude is within one increment from South Pole
    return bbox().south() - increments_.south_north < SOUTH_POLE;
}


size_t RegularLL::size() const {
    ASSERT(ni_);
    ASSERT(nj_);
    return ni_ * nj_;
}


std::vector<Point>&& RegularLL::to_points() const {
    NOTIMP;
}


area::BoundingBox RegularLL::correctBoundingBox(const area::BoundingBox& box, size_t& ni, size_t& nj, const Increments& inc,
                                                const PointLonLat& reference) {
    // Latitude/longitude ranges
    detail::RegularIterator lat{Fraction{box.south()},
                                Fraction{box.north()},
                                Fraction{inc.south_north},
                                Fraction{reference.lat}};
    auto n = lat.b();
    auto s = lat.a();

    nj = lat.n();
    ASSERT(nj > 0);

    detail::RegularIterator lon{Fraction{box.west()},
                                Fraction{box.east()},
                                Fraction{inc.west_east},
                                Fraction{reference.lon},
                                Fraction{GLOBE}};
    auto w = lon.a();
    auto e = lon.b();

    ni = lon.n();
    ASSERT(ni > 0);

    // checks
    ASSERT(w + (ni - 1) * lon.inc() == e || ni * lon.inc() == GLOBE);
    ASSERT(s + (nj - 1) * lat.inc() == n);

    return {n, w, s, e};
}


static const GridRegisterType<RegularLL> __grid_type("regular_ll");


#define POSITIVE_REAL "[+]?([0-9]*[.])?[0-9]+([eE][-+][0-9]+)?"


Configuration* RegularLL::config(const std::string& name) {
    static const std::string pattern("(" POSITIVE_REAL ")/(" POSITIVE_REAL ")");

    auto match = util::regex_match(pattern, name);
    ASSERT(match);
    ASSERT(match.size() == 7);  // because of sub-matches

    auto d = Translator<std::string, double>{};
    std::vector<double> increments{d(match[1]), d(match[4])};

    auto ni = detail::RegularIterator(Fraction(0), Fraction(360), Fraction(increments[0]), Fraction(0), Fraction(360)).n();
    auto nj = detail::RegularIterator(Fraction(-90), Fraction(90), Fraction(increments[1]), Fraction(0)).n();

    return new MappedConfiguration({{"type", "regular_ll"},
                                    {"west_east_increment", increments[0]},
                                    {"south_north_increment", increments[1]},
                                    {"ni", ni},
                                    {"nj", nj}});
}


static const GridRegisterName<RegularLL> __grid_pattern(POSITIVE_REAL "/" POSITIVE_REAL);


#undef POSITIVE_REAL


}  // namespace eckit::geometry::grid
