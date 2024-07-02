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


#include "eckit/geo/grid/HEALPix.h"

#include <algorithm>
#include <bitset>
#include <cmath>
#include <cstdint>
#include <tuple>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/iterator/Reduced.h"
#include "eckit/geo/iterator/Unstructured.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util.h"
#include "eckit/utils/Translator.h"


namespace eckit::geo::grid {


namespace {


struct CodecFijNest {
    static constexpr uint64_t MASKS[] = {0x00000000ffffffff, 0x0000ffff0000ffff, 0x00ff00ff00ff00ff,
                                         0x0f0f0f0f0f0f0f0f, 0x3333333333333333, 0x5555555555555555};

    inline static int nest_encode_bits(int n) {
        auto b = static_cast<uint64_t>(n) & MASKS[0];
        b      = (b ^ (b << 16)) & MASKS[1];
        b      = (b ^ (b << 8)) & MASKS[2];
        b      = (b ^ (b << 4)) & MASKS[3];
        b      = (b ^ (b << 2)) & MASKS[4];
        b      = (b ^ (b << 1)) & MASKS[5];
        return static_cast<int>(b);
    }

    inline static int nest_decode_bits(int n) {
        auto b = static_cast<uint64_t>(n) & MASKS[5];
        b      = (b ^ (b >> 1)) & MASKS[4];
        b      = (b ^ (b >> 2)) & MASKS[3];
        b      = (b ^ (b >> 4)) & MASKS[2];
        b      = (b ^ (b >> 8)) & MASKS[1];
        b      = (b ^ (b >> 16)) & MASKS[0];
        return static_cast<int>(b);
    }

    static std::tuple<int, int, int> nest_to_fij(int n, int k) {
        ASSERT(0 <= n);
        auto f = n >> (2 * k);    // f = n / (Nside * Nside)
        n &= (1 << (2 * k)) - 1;  // n = n % (Nside * Nside)
        auto i = nest_decode_bits(n);
        auto j = nest_decode_bits(n >> 1);
        return {f, i, j};
    }

    static int fij_to_nest(int f, int i, int j, int k) {
        return (f << (2 * k)) + nest_encode_bits(i) + (nest_encode_bits(j) << 1);
    }
};


inline int sqrt(int n) {
    return static_cast<int>(std::sqrt(static_cast<double>(n) + 0.5));
}


// for division result within [0; 3]
inline int div_03(int a, int b) {
    int t = (a >= (b << 1)) ? 1 : 0;
    a -= t * (b << 1);
    return (t << 1) + (a >= b ? 1 : 0);
}


inline bool is_power_of_2(int n) {
    return std::bitset<sizeof(int) * 8>(n).count() == 1;
}


inline int pll(int f) {
    constexpr int __pll[] = {1, 3, 5, 7, 0, 2, 4, 6, 1, 3, 5, 7};
    return __pll[f];
}


class Reorder {
public:
    explicit Reorder(int Nside) :
        Nside_(Nside),
        Npix_(size()),
        Ncap_((Nside * (Nside - 1)) << 1),
        k_(is_power_of_2(Nside_) ? static_cast<int>(std::log2(Nside)) : -1) {
        ASSERT(0 <= k_);  // (specific to nested ordering)
        ASSERT(0 < Nside_);
    }

    int size() const { return 12 * Nside_ * Nside_; }
    int nside() const { return Nside_; }

    int ring_to_nest(int r) const {
        auto to_nest = [&](int f,      //!< base pixel index
                           int ring,   //!< 1-based ring number
                           int Nring,  //!< number of pixels in ring
                           int phi,    //!< index in longitude
                           int shift   //!< if ring's first pixel is not at phi=0
                           ) -> int {
            int r = ((2 + (f >> 2)) << k_) - ring - 1;
            int p = 2 * phi - pll(f) * Nring - shift - 1;
            if (p >= 2 * Nside_) {
                p -= 8 * Nside_;
            }

            int i = (r + p) >> 1;
            int j = (r - p) >> 1;

            ASSERT(f < 12 && i < Nside_ && j < Nside_);
            return CodecFijNest::fij_to_nest(f, i, j, k_);
        };

        if (r < Ncap_) {
            // North polar cap
            int Nring = (1 + sqrt(2 * r + 1)) >> 1;
            int phi   = 1 + r - 2 * Nring * (Nring - 1);
            int r     = Nring;
            int f     = div_03(phi - 1, Nring);

            return to_nest(f, r, Nring, phi, 0);
        }

        if (Npix_ - Ncap_ <= r) {
            // South polar cap
            int Nring = (1 + sqrt(2 * Npix_ - 2 * r - 1)) >> 1;
            int phi   = 1 + r + 2 * Nring * (Nring - 1) + 4 * Nring - Npix_;
            int ring  = 4 * Nside_ - Nring;  // (from South pole)
            int f     = div_03(phi - 1, Nring) + 8;

            return to_nest(f, ring, Nring, phi, 0);
        }

        // Equatorial belt
        int ip  = r - Ncap_;
        int tmp = ip >> (k_ + 2);

        int Nring = Nside_;
        int phi   = ip - tmp * 4 * Nside_ + 1;
        int ring  = tmp + Nside_;

        int ifm = 1 + ((phi - 1 - ((1 + tmp) >> 1)) >> k_);
        int ifp = 1 + ((phi - 1 - ((1 - tmp + 2 * Nside_) >> 1)) >> k_);
        int f   = (ifp == ifm) ? (ifp | 4) : ((ifp < ifm) ? ifp : (ifm + 8));

        return to_nest(f, ring, Nring, phi, ring & 1);
    }

    int nest_to_ring(int n) const {
        auto [f, i, j] = CodecFijNest::nest_to_fij(n, k_);
        ASSERT(f < 12 && i < Nside_ && j < Nside_);

        auto to_ring_local = [&](int f, int i, int j,
                                 int Nring,  //!< number of pixels in ring
                                 int shift   //!< if ring's first pixel is/is not at phi=0
                                 ) -> int {
            Nring >>= 2;
            int r = (pll(f) * Nring + i - j + 1 + shift) / 2 - 1;
            ASSERT(r < 4 * Nring);

            return r < 0 ? r + 4 * Nside_ : r;
        };

        const int ring = ((f >> 2) + 2) * Nside_ - i - j - 1;  // 1-based ring number
        if (ring < Nside_) {
            // North polar cap
            int Nring = 4 * ring;
            int r0    = 2 * ring * (ring - 1);  // index of first ring pixel (ring numbering)

            return r0 + to_ring_local(f, i, j, Nring, 0);
        }

        if (ring < 3 * Nside_) {
            // South polar cap
            int Nring = 4 * Nside_;
            int r0    = Ncap_ + (ring - Nside_) * Nring;  // index of first ring pixel (ring numbering)
            int shift = (ring - Nside_) & 1;

            return r0 + to_ring_local(f, i, j, Nring, shift);
        }

        // Equatorial belt
        int N     = 4 * Nside_ - ring;
        int Nring = 4 * N;
        int r0    = Npix_ - 2 * N * (N + 1);  // index of first ring pixel (ring numbering)

        return r0 + to_ring_local(f, i, j, Nring, 0);
    }

private:
    const int Nside_;  // up to 2^13
    const int Npix_;
    const int Ncap_;
    const int k_;
};


}  // unnamed namespace


HEALPix::HEALPix(const Spec& spec) :
    HEALPix(spec.get_unsigned("Nside"), [](const std::string& str) {
        return str == "ring"     ? Ordering::healpix_ring
               : str == "nested" ? Ordering::healpix_nested
                                 : throw AssertionFailed("HEALPix: supported orderings: ring, nested", Here());
    }(spec.get_string("ordering", "ring"))) {}


HEALPix::HEALPix(size_t Nside, Ordering ordering) : Reduced(area::BoundingBox{}), Nside_(Nside), ordering_(ordering) {
    ASSERT(Nside_ > 0);
    ASSERT_MSG(ordering == Ordering::healpix_ring || ordering == Ordering::healpix_nested,
               "HEALPix: supported orderings: ring, nested");

    if (ordering_ == Ordering::healpix_nested) {
        ASSERT(is_power_of_2(Nside));
    }
}


Renumber HEALPix::reorder(Ordering ordering) const {
    ASSERT_MSG(ordering == Ordering::healpix_ring || ordering == Ordering::healpix_nested,
               "HEALPix: supported orderings: ring, nested");

    if (ordering == ordering_) {
        return Grid::no_reorder(size());
    }

    const Reorder reorder(static_cast<int>(Nside_));
    const auto N = static_cast<int>(size());

    Renumber ren(N);
    for (int i = 0; i < N; ++i) {
        ren[i] = ordering == Ordering::healpix_ring ? reorder.nest_to_ring(i) : reorder.ring_to_nest(i);
    }
    return ren;
}


Grid::iterator HEALPix::cbegin() const {
    return ordering_ == Ordering::healpix_ring ? iterator{new geo::iterator::Reduced(*this, 0)}
                                               : iterator{new geo::iterator::Unstructured(*this, 0, to_points())};
}


Grid::iterator HEALPix::cend() const {
    return ordering_ == Ordering::healpix_ring ? iterator{new geo::iterator::Reduced(*this, size())}
                                               : iterator{new geo::iterator::Unstructured(*this)};
}


size_t HEALPix::ni(size_t j) const {
    ASSERT(j < nj());
    return j < Nside_ ? 4 * (j + 1) : j < 3 * Nside_ ? 4 * Nside_ : ni(nj() - 1 - j);
}


size_t HEALPix::nj() const {
    return 4 * Nside_ - 1;
}


Spec* HEALPix::spec(const std::string& name) {
    ASSERT(name.size() > 1 && (name[0] == 'h' || name[0] == 'H'));

    auto Nside = Translator<std::string, size_t>{}(name.substr(1));
    return new spec::Custom({{"type", "HEALPix"}, {"Nside", Nside}, {"ordering", "ring"}});
}


size_t HEALPix::size() const {
    return 12 * Nside_ * Nside_;
}


std::vector<Point> HEALPix::to_points() const {
    const auto points = Reduced::to_points();

    if (ordering_ == Ordering::healpix_ring) {
        return points;
    }

    std::vector<Point> points_nested;
    points_nested.reserve(size());

    const Reorder reorder(static_cast<int>(Nside_));
    for (size_t i = 0; i < size(); ++i) {
        points_nested.emplace_back(std::get<PointLonLat>(points[reorder.nest_to_ring(static_cast<int>(i))]));
    }

    return points_nested;
}


std::pair<std::vector<double>, std::vector<double>> HEALPix::to_latlon() const {
    std::pair<std::vector<double>, std::vector<double>> latlon;
    latlon.first.reserve(size());
    latlon.second.reserve(size());

    for (const auto& p : to_points()) {
        const auto& q = std::get<PointLonLat>(p);
        latlon.first.push_back(q.lat);
        latlon.second.push_back(q.lon);
    }

    return latlon;
}


const std::vector<double>& HEALPix::latitudes() const {
    const auto Nj = nj();

    if (latitudes_.empty()) {
        latitudes_.resize(Nj);

        auto i = latitudes_.begin();
        auto j = latitudes_.rbegin();
        for (size_t ring = 1; ring < 2 * Nside_; ++ring, ++i, ++j) {
            const auto f = ring < Nside_
                               ? 1. - static_cast<double>(ring * ring) / (3 * static_cast<double>(Nside_ * Nside_))
                               : 4. / 3. - 2 * static_cast<double>(ring) / (3 * static_cast<double>(Nside_));

            *i = 90. - util::RADIAN_TO_DEGREE * std::acos(f);
            *j = -*i;
        }
        *i = 0.;
    }

    ASSERT(latitudes_.size() == Nj);
    return latitudes_;
}


std::vector<double> HEALPix::longitudes(size_t j) const {
    const auto Ni    = ni(j);
    const auto step  = 360. / static_cast<double>(Ni);
    const auto start = j < Nside_ || 3 * Nside_ - 1 < j || static_cast<bool>((j + Nside_) % 2) ? step / 2. : 0.;

    std::vector<double> lons(Ni);
    std::generate_n(lons.begin(), Ni,
                    [start, step, n = 0ULL]() mutable { return start + static_cast<double>(n++) * step; });

    return lons;
}


void HEALPix::fill_spec(spec::Custom& custom) const {
    custom.set("grid", "H" + std::to_string(Nside_));
    custom.set("ordering", ordering_ == Ordering::healpix_ring ? "ring" : "nested");
}


static const GridRegisterType<HEALPix> GRIDTYPE1("HEALPix");
static const GridRegisterType<HEALPix> GRIDTYPE2("healpix");
static const GridRegisterName<HEALPix> GRIDNAME("[hH][1-9][0-9]*");


}  // namespace eckit::geo::grid
