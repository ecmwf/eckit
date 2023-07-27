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


#include "eckit/geometry/grid/ReducedGG.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <numeric>
#include <ostream>
#include <set>
#include <utility>

#include "eckit/config/MappedConfiguration.h"
#include "eckit/geometry/Iterator.h"
#include "eckit/geometry/Projection.h"
#include "eckit/geometry/grid/GaussianIterator.h"
#include "eckit/types/Fraction.h"
#include "eckit/utils/Translator.h"


namespace eckit::geometry::grid {


ReducedGG::ReducedGG(const Configuration& config) :
    Gaussian(config), k_(0), Nj_(N_ * 2) {

    // adjust latitudes, longitudes and re-set bounding box
    auto n = bbox().north();
    auto s = bbox().south();
    correctSouthNorth(s, n);

    pl_type pl;
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


void ReducedGG::correctWestEast(double& w, double& e) const {
    ASSERT(w <= e);

    const Fraction smallestIncrement = getSmallestIncrement();
    ASSERT(smallestIncrement > 0);

    if (angleApproximatelyEqual(GREENWICH, w) && (angleApproximatelyEqual(GLOBE - smallestIncrement, e - w) || GLOBE - smallestIncrement < e - w || (e != w && PointLonLat::normalise_angle_to_minimum(e, w) == w))) {

        w = GREENWICH;
        e = GLOBE - smallestIncrement;
    }
    else {

        const Fraction west{w};
        const Fraction east{e};
        Fraction W = west;
        Fraction E = east;

        bool first = true;
        std::set<long> NiTried;

        const auto& pl = pls();
        for (size_t j = k_; j < k_ + Nj_; ++j) {

            // crop longitude-wise, track distinct attempts
            const long Ni(pl[j]);
            ASSERT(Ni >= 2);
            if (NiTried.insert(Ni).second) {

                Fraction inc = Fraction{GLOBE} / Ni;

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


const pl_type& ReducedGG::pls() const {
    ASSERT(pl_.size() == N_ * 2);
    ASSERT(pl_.size() >= k_ + Nj_);
    ASSERT(Nj_ > 0);

    return pl_;
}


void ReducedGG::setNj(pl_type pl, double s, double n) {
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


size_t ReducedGG::size() const {
    const auto& pl = pls();
    return size_t(std::accumulate(pl.begin(), pl.end(), pl_type::value_type{0}));
}


Fraction ReducedGG::getSmallestIncrement() const {
    using distance_t = std::make_signed<size_t>::type;

    const auto& pl = pls();
    auto maxpl     = *std::max_element(pl.begin() + distance_t(k_), pl.begin() + distance_t(k_ + Nj_));
    ASSERT(maxpl >= 2);

    const Fraction globe_f(GLOBE);
    return globe_f / maxpl;
}


bool ReducedGG::isPeriodicWestEast() const {
    auto inc = getSmallestIncrement();
    return bbox().east() - bbox().west() + inc >= GLOBE;
}


void ReducedGG::print(std::ostream& out) const {
    out << "ReducedGG[N=" << N_ << ",bbox=" << bbox() << "]";
}


struct ReducedGGClassic : ReducedGG {
    ReducedGGClassic(size_t N, const area::BoundingBox& box = {}) :
        ReducedGG(N, box) {

        // adjust latitudes, longitudes and re-set bounding box
        auto n = box.north();
        auto s = box.south();
        correctSouthNorth(s, n);

        setNj(pls("N" + std::to_string(N_)), s, n);

        auto w = box.west();
        auto e = box.east();
        correctWestEast(w, e);

        bbox({n, w, s, e});
    }
};


struct ReducedGGFromPL : ReducedGG {
    ReducedGGFromPL(const Configuration& config) :
        ReducedGG(config) {}
    ReducedGGFromPL(size_t N, const pl_type& pl, const area::BoundingBox& box = {}) :
        ReducedGG(N, pl, box) {}
};


struct ReducedGGOctahedral : ReducedGG {
    ReducedGGOctahedral(size_t N, const area::BoundingBox& box = {}) :
        ReducedGG(N, box) {

        // adjust latitudes, longitudes and re-set bounding box
        auto [n, w, s, e] = bbox().deconstruct();

        correctSouthNorth(s, n);

        setNj(pls("O" + std::to_string(N_)), s, n);

        correctWestEast(w, e);

        bbox({n, w, s, e});
    }

    static Configuration* config(const std::string& name) {
        Translator<std::string, size_t> trans;

        return new MappedConfiguration({{"type", "reduced_gg"},
                                        {"N", trans(name.substr(1))}});
    }
};


static const GridRegisterType<ReducedGGFromPL> __grid_type("reduced_gg");
static const GridRegisterName<ReducedGGOctahedral> __grid_pattern("[oO][1-9][0-9]*");


}  // namespace eckit::geometry::grid
