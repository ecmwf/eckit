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


#include "eckit/geo/grid/Reduced.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <numeric>
#include <set>
#include <utility>

#include "eckit/geo/grid/GaussianIterator.h"
#include "eckit/types/Fraction.h"


namespace eckit::geo::grid {


template <typename T>
std::vector<long> pl_convert(const T& nx) {
    ASSERT(!nx.empty());
    std::vector<long> pl(nx.size());
    std::transform(nx.begin(), nx.end(), pl.begin(), [](typename T::value_type p) { return long(p); });
    return pl;
}


template <>
std::vector<long> pl_convert(const std::vector<long>& nx) {
    ASSERT(!nx.empty());
    return nx;
}


Reduced::Reduced(const Configuration& config) :
    Gaussian(config), k_(0), Nj_(N_ * 2) {

    // adjust latitudes, longitudes and re-set bounding box
    auto n = bbox().north();
    auto s = bbox().south();
    correctSouthNorth(s, n);

    std::vector<long> pl;
    ASSERT(config.get("pl", pl));

    // if pl isn't global (from file!) insert leading/trailing 0's
    const auto& lats = latitudes();
    if (n < lats.front() || s > lats.back()) {
        size_t k  = 0;
        size_t nj = 0;
        for (auto lat : lats) {
            if (n < lat) {
                ++k;
            }
            else if (s <= lat) {
                ASSERT(pl[nj] >= 2);
                ++nj;
            }
            else {
                break;
            }
        }
        ASSERT(k + nj <= N_ * 2);

        if (k > 0) {
            pl.reserve(N_ * 2);
            pl.insert(pl.begin(), k, 0);
        }
        pl.resize(N_ * 2, 0);
    }

    setNj(pl, s, n);

    auto w = bbox().west();
    auto e = bbox().east();
    correctWestEast(w, e);

    bbox({n, w, s, e});
}


Reduced::Reduced(size_t N, const std::vector<long>& pl, const BoundingBox& bbox, double angularPrecision) :
    Gaussian(N, bbox, angularPrecision), k_(0), Nj_(N_ * 2) {
    setNj(pl, bbox.south(), bbox.north());
}


Reduced::Reduced(size_t N, const BoundingBox& bbox, double angularPrecision) :
    Gaussian(N, bbox, angularPrecision), k_(0), Nj_(N * 2) {
    // derived classes must set k_, Nj_ using this constructor
}


void Reduced::correctWestEast(double& w, double& e) const {
    ASSERT(w <= e);

    const Fraction smallestIncrement = getSmallestIncrement();
    ASSERT(smallestIncrement > 0);

    if (angleApproximatelyEqual(GREENWICH, w) && (angleApproximatelyEqual(GLOBE - smallestIncrement, e - w) || GLOBE - smallestIncrement < e - w || (e != w && e.normalise(w) == w))) {

        w = GREENWICH;
        e = GLOBE - smallestIncrement;
    }
    else {

        const Fraction west = w.fraction();
        const Fraction east = e.fraction();
        Fraction W          = west;
        Fraction E          = east;

        bool first = true;
        std::set<long> NiTried;

        const auto& pl = pls();
        for (size_t j = k_; j < k_ + Nj_; ++j) {

            // crop longitude-wise, track distinct attempts
            const long Ni(pl[j]);
            ASSERT(Ni >= 2);
            if (NiTried.insert(Ni).second) {

                Fraction inc = GLOBE.fraction() / Ni;

                Fraction::value_type Nw = (west / inc).integralPart();
                if (Nw * inc < west) {
                    Nw += 1;
                }

                Fraction::value_type Ne = (east / inc).integralPart();
                if (Ne * inc > east || Nw + Ne == Ni) {
                    Ne -= 1;
                }

                if (Nw <= Ne) {
                    ASSERT(w <= Longitude(Nw * inc));
                    ASSERT(Longitude(Ne * inc) <= e);

                    if (W > double(Nw * inc) || first) {
                        W = Nw * inc;
                    }
                    if (E < double(Ne * inc) || first) {
                        E = Ne * inc;
                    }
                    first = false;
                }
            }
        }

        ASSERT(!first);
        ASSERT(W <= E);
        w = W;
        e = E;
    }
}


Iterator* Reduced::unrotatedIterator() const {
    auto pl = pls();
    return new GaussianIterator(latitudes(), std::move(pl), bbox(), N_, Nj_, k_);
}


Iterator* Reduced::rotatedIterator(const Rotation& rotation) const {
    auto pl = pls();
    return new GaussianIterator(latitudes(), std::move(pl), bbox(), N_, Nj_, k_, rotation);
}


const std::vector<long>& Reduced::pls() const {
    ASSERT(pl_.size() == N_ * 2);
    ASSERT(pl_.size() >= k_ + Nj_);
    ASSERT(Nj_ > 0);

    return pl_;
}


std::vector<long> Reduced::pls(const std::string& name) {
    atlas::ReducedGaussianGrid grid(name);
    ASSERT(grid);
    return pl_convert(grid.nx());
}


void Reduced::setNj(std::vector<long> pl, double s, double n) {
    ASSERT(0 < N_ && N_ * 2 == pl.size());

    // position to first latitude and first/last longitude
    // NOTE: latitudes() spans the globe, sorted from North-to-South, k_ positions the North
    // NOTE: pl spans the globe
    pl_ = pl;
    k_  = 0;
    Nj_ = N_ * 2;

    const auto& lats = latitudes();
    if (n < lats.front() || s > lats.back()) {
        Nj_ = 0;
        for (const auto& lat : lats) {
            Latitude ll(lat);
            if (n < ll && !angleApproximatelyEqual(n, ll)) {
                ++k_;
            }
            else if (s < ll || angleApproximatelyEqual(s, ll)) {
                ASSERT(pl[k_ + Nj_] >= 2);
                ++Nj_;
            }
            else {
                break;
            }
        }
    }

    // check internal assumptions
    pls();
}


size_t Reduced::numberOfPoints() const {
    if (isGlobal()) {
        const auto& pl = pls();
        return size_t(std::accumulate(pl.begin(), pl.end(), 0L));
    }

    size_t total = 0;
    for (const std::unique_ptr<Iterator> it(iterator()); it->next();) {
        total++;
    }
    return total;
}


bool Reduced::isPeriodicWestEast() const {
    auto inc = getSmallestIncrement();
    return bbox_.east() - bbox_.west() + inc >= GLOBE;
}


}  // namespace eckit::geo::grid
