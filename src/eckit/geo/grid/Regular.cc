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

#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"


namespace eckit::geo::grid {

Regular::Regular(const Configuration& config) :
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

Regular::Regular(size_t N, const BoundingBox& box, double angularPrecision) :
    Gaussian(N, box, angularPrecision), k_(0), Ni_(0), Nj_(0) {

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

void Regular::correctWestEast(double& w, double& e) const {
    ASSERT(w <= e);

    auto inc = getSmallestIncrement();
    ASSERT(inc > 0);

    if (angleApproximatelyEqual(GREENWICH, w) && (angleApproximatelyEqual(GLOBE - inc, e - w) || GLOBE - inc < e - w || (e != w && e.normalise(w) == w))) {

        w = GREENWICH;
        e = GLOBE - inc;
    }
    else {

        const Fraction west = w;
        const Fraction east = e;

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
}

bool Regular::sameAs(const Grid& other) const {
    const auto* o = dynamic_cast<const Regular*>(&other);
    return (o != nullptr) && (N_ == o->N_) && (bbox() == o->bbox());
}

eckit::Fraction Regular::getSmallestIncrement() const {
    ASSERT(N_);
    return {90, Fraction::value_type(N_)};
}

size_t Regular::numberOfPoints() const {
    ASSERT(Ni_);
    ASSERT(Nj_);
    return Ni_ * Nj_;
}

bool Regular::isPeriodicWestEast() const {
    auto inc = getSmallestIncrement();
    return bbox().east() - bbox().west() + inc >= GLOBE;
}

void Regular::setNiNj() {
    ASSERT(N_);

    const auto inc   = getSmallestIncrement();
    const auto& lats = latitudes();

    double west  = bbox().west();
    double east  = bbox().east();
    double south = bbox().south();
    double north = bbox().north();

    Ni_ = N_ * 4;

    if (east - west + inc < GLOBE) {

        auto w  = west.fraction();
        auto Nw = (w / inc).integralPart();
        if (Nw * inc < w) {
            Nw += 1;
        }

        auto e  = east.fraction();
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

    if (north < lats.front() || south > lats.back()) {
        Nj_ = 0;
        for (auto lat : lats) {
            if (north < lat && !angleApproximatelyEqual(north, lat)) {
                ++k_;
            }
            else if (south <= lat || angleApproximatelyEqual(south, lat)) {
                ++Nj_;
            }
            else {
                break;
            }
        }
        ASSERT(Nj_ > 0);
    }

    Log::debug() << "Regular::setNiNj: Ni*Nj = " << Ni_ << " * " << Nj_ << " = " << (Ni_ * Nj_) << std::endl;
}

}  // namespace eckit::geo::grid
