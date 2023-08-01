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
#include <numeric>
#include <set>
#include <type_traits>

#include "eckit/config/MappedConfiguration.h"
#include "eckit/geometry/Iterator.h"
#include "eckit/geometry/Projection.h"
#include "eckit/geometry/grid/GaussianIterator.h"
#include "eckit/types/Fraction.h"
#include "eckit/utils/Translator.h"


namespace eckit::geometry::grid {


static Fraction smallest_increment(const pl_type& pl) {
    auto maxpl = *std::max_element(pl.begin(), pl.end());
    ASSERT(maxpl >= 2);
    return Fraction(GLOBE) / maxpl;
}


ReducedGG::ReducedGG(const Configuration& config) :
    ReducedGG(config.getUnsigned("N"), config.getLongVector("pl")) {}


ReducedGG::ReducedGG(size_t N, const pl_type& pl) :
    Gaussian(N), pl_(pl), k_(0), Nj_(N_ * 2) {
    // adjust latitudes, longitudes and re-set bounding box
    auto n = bbox().north();
    auto s = bbox().south();
    correctSouthNorth(s, n);

    const auto& lats = latitudes();

    // check internal assumptions
    ASSERT(pl_.size() == N_ * 2);
    ASSERT(pl_.size() >= k_ + Nj_);
    ASSERT(Nj_ > 0);

    // set-up North/South
    ASSERT(0 < N_ && N_ * 2 == pl_.size());

    // position to first latitude and first/last longitude
    // NOTE: latitudes() spans the globe, sorted from North-to-South, k_ positions the North
    // NOTE: pl spans the globe
    k_  = 0;
    Nj_ = N_ * 2;

    if (n < lats.front() || s > lats.back()) {
        Nj_ = 0;
        for (const auto& lat : lats) {
            Latitude ll(lat);
            if (n < ll && !angleApproximatelyEqual(n, ll)) {
                ++k_;
            }
            else if (s < ll || angleApproximatelyEqual(s, ll)) {
                ASSERT(pl_[k_ + Nj_] >= 2);
                ++Nj_;
            }
            else {
                break;
            }
        }
    }


    // correct West/East
    auto w = bbox().west();
    auto e = bbox().east();
    ASSERT(w <= e);

    const Fraction inc = smallest_increment(pl_);
    ASSERT(inc > 0);

    if (angleApproximatelyEqual(GREENWICH, w) && (angleApproximatelyEqual(GLOBE - inc, e - w) || GLOBE - inc < e - w || (e != w && PointLonLat::normalise_angle_to_minimum(e, w) == w))) {
        w = GREENWICH;
        e = GLOBE - inc;
    }
    else {
        const Fraction west{w};
        const Fraction east{e};
        Fraction W = west;
        Fraction E = east;

        bool first = true;
        std::set<long> NiTried;

        for (size_t j = k_; j < k_ + Nj_; ++j) {

            // crop longitude-wise, track distinct attempts
            const long Ni(pl_[j]);
            ASSERT(Ni >= 2);
            if (NiTried.insert(Ni).second) {
                Fraction inc = Fraction{GLOBE} / Ni;

                auto Nw = (west / inc).integralPart();
                if (Nw * inc < west) {
                    Nw += 1;
                }

                auto Ne = (east / inc).integralPart();
                if (Ne * inc > east || Nw + Ne == Ni) {
                    Ne -= 1;
                }

                if (Nw <= Ne) {
                    ASSERT(w <= Longitude(Nw * inc));
                    ASSERT(Longitude(Ne * inc) <= e);

                    if (W > static_cast<double>(Nw * inc) || first) {
                        W = Nw * inc;
                    }
                    if (E < static_cast<double>(Ne * inc) || first) {
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


    // bounding box
    bbox({n, w, s, e});


    // accumulated pl
    placc_.resize(1 + pl_.size());
    placc_.front() = 0;

    auto i = pl_.begin();
    for (auto j = placc_.begin(), k = j + 1; k != placc_.end(); ++i, ++j, ++k) {
        *k = *i + *j;
    }
}


size_t ReducedGG::size() const {
    return static_cast<size_t>(placc_.back());
}


bool ReducedGG::isPeriodicWestEast() const {
    auto inc = smallest_increment(pl_);
    return bbox().east() - bbox().west() + inc >= GLOBE;
}


struct ReducedGGClassical {
    static Configuration* config(const std::string& name) {
        auto N = Translator<std::string, size_t>{}(name.substr(1));
        return new MappedConfiguration({{"type", "reduced_gg"},
                                        {"N", N},
                                        {"pl", util::reduced_classical_pl(N)}});
    }
};


struct ReducedGGOctahedral {
    static Configuration* config(const std::string& name) {
        auto N = Translator<std::string, size_t>{}(name.substr(1));
        return new MappedConfiguration({{"type", "reduced_gg"},
                                        {"N", N},
                                        {"pl", util::reduced_octahedral_pl(N)}});
    }
};


static const GridRegisterType<ReducedGG> __grid_type("reduced_gg");
static const GridRegisterName<ReducedGGClassical> __grid_pattern_1("[nN][1-9][0-9]*");
static const GridRegisterName<ReducedGGOctahedral> __grid_pattern_2("[oO][1-9][0-9]*");


}  // namespace eckit::geometry::grid
