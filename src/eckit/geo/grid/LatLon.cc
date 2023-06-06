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


#include "eckit/geo/grid/LatLon.h"

#include <memory>
#include <ostream>
#include <sstream>

#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"


namespace eckit::geo::grid {


LatLon::LatLon(const Configuration& config) :
    Grid(config), increments_(parametrisation), reference_(bbox_.south(), bbox_.west()), ni_(0), nj_(0) {
    correctBoundingBox(bbox_, ni_, nj_, increments_, reference_);
    ASSERT(ni_ != 0);
    ASSERT(nj_ != 0);

    // confirm Ni/Nj from config (input)
    size_t ni = 0;
    size_t nj = 0;
    ASSERT(config.get("Ni", ni));
    ASSERT(config.get("Nj", nj));

    Log::debug() << "LatLon:"
                    "\n\t"
                    "(Ni, Nj) = ("
                 << ni_ << ", " << nj_
                 << ") calculated"
                    "\n\t"
                    "(Ni, Nj) = ("
                 << ni << ", " << nj << ") from config" << std::endl;

    ASSERT(ni == ni_);
    ASSERT(nj == nj_);
}


LatLon::LatLon(const Increments& increments, const BoundingBox& bbox, const PointLonLat& reference) :
    Grid(bbox), increments_(increments), reference_(reference), ni_(0), nj_(0) {
    correctBoundingBox(bbox_, ni_, nj_, increments_, reference_);
    ASSERT(ni_ != 0);
    ASSERT(nj_ != 0);
}


LatLon::~LatLon() = default;


void LatLon::reorder(long scanningMode) const {
    grib_reorder(values, scanningMode, ni_, nj_);
}


void LatLon::print(std::ostream& out) const {
    out << "LatLon["
        << "bbox=" << bbox_ << ",increments=" << increments_ << ",ni=" << ni_ << ",nj=" << nj_ << "]";
}


bool LatLon::sameAs(const Grid& other) const {
    const auto* o = dynamic_cast<const LatLon*>(&other);
    return (o != nullptr) && (bbox_ == o->bbox_) && (increments_ == o->increments_);
}


bool LatLon::isPeriodicWestEast() const {

    // if range West-East is within one increment (or greater than) 360 degree
    double inc = increments_.west_east().longitude();
    return bbox_.east() - bbox_.west() + inc >= GLOBE;
}


bool LatLon::includesNorthPole() const {

    // if North latitude is within one increment from North Pole
    double inc = increments_.south_north().latitude();
    return bbox_.north() + inc > NORTH_POLE;
}


bool LatLon::includesSouthPole() const {

    // if South latitude is within one increment from South Pole
    double inc = increments_.south_north().latitude();
    return bbox_.south() - inc < SOUTH_POLE;
}


size_t LatLon::numberOfPoints() const {
    ASSERT(ni_);
    ASSERT(nj_);
    return ni_ * nj_;
}


const LatLon* LatLon::croppedGrid(const BoundingBox& /*unused*/) const {
    std::ostringstream os;
    os << "LatLon::croppedGrid() not implemented for " << *this;
    throw SeriousBug(os.str());
}


LatLon::LatLonIterator::LatLonIterator(size_t ni, size_t nj, Latitude north, Longitude west,
                                       const Increments& increments) :
    ni_(ni),
    nj_(nj),
    north_(north.fraction()),
    west_(west.fraction()),
    we_(increments.west_east().longitude().fraction()),
    ns_(increments.south_north().latitude().fraction()),
    i_(0),
    j_(0),
    count_(0),
    first_(true) {
    lat_      = north_;
    lon_      = west_;
    latValue_ = lat_;
    lonValue_ = lon_;
}


LatLon::LatLonIterator::~LatLonIterator() {
    auto count = count_ + (i_ > 0 || j_ > 0 ? 1 : 0);
    ASSERT(count == ni_ * nj_);
}


void LatLon::LatLonIterator::print(std::ostream& out) const {
    out << "LatLonIterator["
        << "ni=" << ni_ << ",nj=" << nj_ << ",north=" << north_ << ",west=" << west_ << ",we=" << we_ << ",ns=" << ns_
        << ",i=" << i_ << ",j=" << j_ << ",count=" << count_ << "]";
}


bool LatLon::LatLonIterator::next(double& lat, double& lon) {
    if (j_ < nj_) {
        if (i_ < ni_) {
            lat = latValue_;
            lon = lonValue_;

            lon_ += we_;

            if (first_) {
                first_ = false;
            }
            else {
                count_++;
            }

            if (++i_ == ni_) {
                j_++;
                i_ = 0;
                lat_ -= ns_;
                lon_      = west_;
                latValue_ = lat_;
            }

            lonValue_ = lon_;

            return true;
        }
    }
    return false;
}


void LatLon::correctBoundingBox(BoundingBox& bbox, size_t& ni, size_t& nj, const Increments& inc,
                                const PointLonLat& reference) {
    using iterator::detail::RegularIterator;

    // Latitude/longitude ranges
    RegularIterator lat{bbox.south().fraction(), bbox.north().fraction(), inc.south_north().latitude().fraction(),
                        reference.lat().fraction()};
    auto n = lat.b();
    auto s = lat.a();

    nj = lat.n();
    ASSERT(nj > 0);

    RegularIterator lon{bbox.west().fraction(), bbox.east().fraction(), inc.west_east().longitude().fraction(),
                        reference.lon().fraction(), GLOBE.fraction()};
    auto w = lon.a();
    auto e = lon.b();

    ni = lon.n();
    ASSERT(ni > 0);

    // checks
    ASSERT(w + (ni - 1) * lon.inc() == e || ni * lon.inc() == GLOBE.fraction());
    ASSERT(s + (nj - 1) * lat.inc() == n);

    bbox = {n, w, s, e};
}


}  // namespace eckit::geo::grid
