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


#include "eckit/grid/detail/RegularGG.h"

#include <algorithm>
#include <ostream>
#include <utility>

#include "eckit/grid/detail/GaussianIterator.h"
#include "eckit/grid/detail/RegularGG.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"


namespace eckit::grid::grid {


RegularGG::RegularGG(const Configuration& config) :
    Gaussian(config), k_(0), Ni_(0), Nj_(0) {
    // adjust latitudes, longitudes and re-set bounding box
    auto n = bbox().north();
    auto s = bbox().south();
    correctSouthNorth(s, n);

    auto e = bbox().east();
    auto w = bbox().west();
    correctWestEast(w, e);

    bbox({n, w, s, e});

    setNiNj();
}


RegularGG::RegularGG(size_t N, const BoundingBox& box) :
    Gaussian(N, box), k_(0), Ni_(0), Nj_(0) {

    // adjust latitudes, longitudes and re-set bounding box
    auto n = box.north();
    auto s = box.south();
    correctSouthNorth(s, n);

    auto w = box.west();
    auto e = box.east();
    correctWestEast(w, e);

    bbox({n, w, s, e});

    setNiNj();
}


void RegularGG::correctWestEast(double& w, double& e) const {
    ASSERT(w <= e);

    auto inc = getSmallestIncrement();
    ASSERT(inc > 0);

    if (angleApproximatelyEqual(GREENWICH, w) && (angleApproximatelyEqual(GLOBE - inc, e - w) || GLOBE - inc < e - w || (e != w && PointLonLat::normalise_angle_to_minimum(e, w) == w))) {
        w = GREENWICH;
        e = GLOBE - inc;
        return;
    }

    const Fraction west{w};
    const Fraction east{e};

    Fraction::value_type Nw = (west / inc).integralPart();
    if (Nw * inc < west) {
        Nw += 1;
    }

    Fraction::value_type Ne = (east / inc).integralPart();
    if (Ne * inc > east) {
        Ne -= 1;
    }

    ASSERT(Nw <= Ne);
    w = Nw * inc;
    e = Ne * inc;
}


Fraction RegularGG::getSmallestIncrement() const {
    ASSERT(N_);
    return {90, Fraction::value_type(N_)};
}


size_t RegularGG::numberOfPoints() const {
    ASSERT(Ni_);
    ASSERT(Nj_);
    return Ni_ * Nj_;
}


bool RegularGG::isPeriodicWestEast() const {
    auto inc = getSmallestIncrement();
    return bbox().east() - bbox().west() + inc >= GLOBE;
}


void RegularGG::setNiNj() {
    ASSERT(N_);

    const auto inc   = getSmallestIncrement();
    const auto& lats = latitudes();

    Fraction w(bbox().west());
    Fraction e(bbox().east());
    Fraction s(bbox().south());
    Fraction n(bbox().north());

    Ni_ = N_ * 4;

    if (e - w + inc < GLOBE) {

        auto Nw = (w / inc).integralPart();
        if (Nw * inc < w) {
            Nw += 1;
        }

        auto Ne = (e / inc).integralPart();
        if (Ne * inc > e) {
            Ne -= 1;
        }

        ASSERT(Ne - Nw + 1 > 0);
        Ni_ = size_t(Ne - Nw + 1);

        ASSERT(2 <= Ni_ && Ni_ <= N_ * 4);
    }

    k_  = 0;
    Nj_ = N_ * 2;

    if (n < lats.front() || s > lats.back()) {
        Nj_ = 0;
        for (auto lat : lats) {
            if (n < lat && !angleApproximatelyEqual(n, lat)) {
                ++k_;
            }
            else if (s <= lat || angleApproximatelyEqual(s, lat)) {
                ++Nj_;
            }
            else {
                break;
            }
        }
        ASSERT(Nj_ > 0);
    }
}


void RegularGG::print(std::ostream& out) const {
    out << "RegularGG["
           "N="
        << N_ << ",Ni=" << Ni_ << ",Nj=" << Nj_ << ",bbox=" << bbox() << "]";
}


Iterator* RegularGG::iterator() const {
    std::vector<long> pl(N_ * 2, long(4 * N_));
    return new GaussianIterator(latitudes(), std::move(pl), bbox(), N_, Nj_, k_);
}


Renumber RegularGG::crop(BoundingBox&) const {
    NOTIMP;
}


Renumber RegularGG::reorder(long /*scanningMode*/) const {
    NOTIMP;
}


static const GridBuilder<RegularGG> reducedGG("regular_gg");


}  // namespace eckit::grid::grid
