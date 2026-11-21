// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/order/HEALPix.h"

#include <cmath>
#include <cstdint>
#include <numeric>
#include <tuple>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/grid/reduced/HEALPix.h"
#include "eckit/utils/SafeCasts.h"


namespace eckit::geo::order {


const HEALPix::order_type HEALPix::RING   = "ring";
const HEALPix::order_type HEALPix::NESTED = "nested";


namespace {


constexpr uint64_t MASKS[] = {0x00000000ffffffff, 0x0000ffff0000ffff, 0x00ff00ff00ff00ff,
                              0x0f0f0f0f0f0f0f0f, 0x3333333333333333, 0x5555555555555555};


inline int nest_encode_bits(int n) {
    auto b = uint64_t{into_unsigned(n)} & MASKS[0];
    b      = (b ^ (b << 16)) & MASKS[1];
    b      = (b ^ (b << 8)) & MASKS[2];
    b      = (b ^ (b << 4)) & MASKS[3];
    b      = (b ^ (b << 2)) & MASKS[4];
    b      = (b ^ (b << 1)) & MASKS[5];
    return static_cast<int>(b);
}


inline int nest_decode_bits(int n) {
    auto b = uint64_t{into_unsigned(n)} & MASKS[5];
    b      = (b ^ (b >> 1)) & MASKS[4];
    b      = (b ^ (b >> 2)) & MASKS[3];
    b      = (b ^ (b >> 4)) & MASKS[2];
    b      = (b ^ (b >> 8)) & MASKS[1];
    b      = (b ^ (b >> 16)) & MASKS[0];
    return static_cast<int>(b);
}


std::tuple<int, int, int> nest_to_fij(int n, int k) {
    ASSERT(0 <= n);
    auto f = n >> (2 * k);    // f = n / (Nside * Nside)
    n &= (1 << (2 * k)) - 1;  // n = n % (Nside * Nside)
    auto i = nest_decode_bits(n);
    auto j = nest_decode_bits(n >> 1);
    return {f, i, j};
}


int fij_to_nest(int f, int i, int j, int k) {
    return (f << (2 * k)) + nest_encode_bits(i) + (nest_encode_bits(j) << 1);
}


inline int sqrt(int n) {
    return static_cast<int>(std::sqrt(static_cast<double>(n) + 0.5));
}


// for division result within [0; 3]
inline int div_03(int a, int b) {
    int t = (a >= (b << 1)) ? 1 : 0;
    a -= t * (b << 1);
    return (t << 1) + (a >= b ? 1 : 0);
}


int valid_nested_nside(size_t Nside) {
    if (Nside == 0 || Nside > (1U << 13) || (Nside & (Nside - 1)) != 0) {
        throw exception::OrderError("HEALPix: nested order of Nside a power of 2, up to 2^13, expected", Here());
    }
    return static_cast<int>(Nside);
}


inline int pll(int f) {
    constexpr int __pll[] = {1, 3, 5, 7, 0, 2, 4, 6, 1, 3, 5, 7};
    return __pll[f];
}


}  // namespace


HEALPix::HEALPix(size_t Nside, const order_type& order) : Nside_(Nside), order_(order) {
    if (order_ != RING && order_ != NESTED) {
        throw exception::OrderError("HEALPix: supported ordering: ring, nested", Here());
    }

    if (order_ == NESTED) {
        renumber_.emplace(Nside_);
    }
}


HEALPix::HEALPix(const Spec& spec) : HEALPix(spec.get_unsigned("Nside"), spec.get_string("order", order_default())) {}


const HEALPix::Renumber& HEALPix::renumber() const {
    ASSERT(renumber_);
    return *renumber_;
}


HEALPix::Renumber::Renumber(size_t Nside) :
    Nside_(valid_nested_nside(Nside)),
    Npix_(static_cast<int>(grid::reduced::HEALPix::size_from_nside(Nside))),
    Ncap_((Nside_ * (Nside_ - 1)) << 1),
    k_(static_cast<int>(std::log2(Nside_))) {}


size_t HEALPix::Renumber::ring_to_nest(size_t index) const {
    const auto _r = static_cast<int>(index);

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
        return fij_to_nest(f, i, j, k_);
    };

    if (Nside_ == 1) {
        return into_unsigned(_r);
    }

    if (_r < Ncap_) {
        // North polar cap
        int Nring = (1 + sqrt(2 * _r + 1)) >> 1;
        int phi   = 1 + _r - 2 * Nring * (Nring - 1);
        int r     = Nring;
        int f     = div_03(phi - 1, Nring);

        return into_unsigned(to_nest(f, r, Nring, phi, 0));
    }

    if (Npix_ - Ncap_ <= _r) {
        // South polar cap
        int Nring = (1 + sqrt(2 * Npix_ - 2 * _r - 1)) >> 1;
        int phi   = 1 + _r + 2 * Nring * (Nring - 1) + 4 * Nring - Npix_;
        int ring  = 4 * Nside_ - Nring;  // (from South pole)
        int f     = div_03(phi - 1, Nring) + 8;

        return into_unsigned(to_nest(f, ring, Nring, phi, 0));
    }

    // Equatorial belt
    int ip  = _r - Ncap_;
    int tmp = ip >> (k_ + 2);

    int Nring = Nside_;
    int phi   = ip - tmp * 4 * Nside_ + 1;
    int ring  = tmp + Nside_;

    int ifm = 1 + ((phi - 1 - ((1 + tmp) >> 1)) >> k_);
    int ifp = 1 + ((phi - 1 - ((1 - tmp + 2 * Nside_) >> 1)) >> k_);
    int f   = (ifp == ifm) ? (ifp | 4) : ((ifp < ifm) ? ifp : (ifm + 8));

    return into_unsigned(to_nest(f, ring, Nring, phi, ring & 1));
}


std::pair<size_t, size_t> HEALPix::Renumber::nest_to_row(size_t index) const {
    const auto n = static_cast<int>(index);

    auto [f, i, j] = nest_to_fij(n, k_);
    ASSERT(f < 12 && i < Nside_ && j < Nside_);

    const int ring = ((f >> 2) + 2) * Nside_ - i - j - 1;  // 1-based ring number

    // number of pixels in ring (polar caps, equatorial belt), and if ring's first pixel is/is not at phi=0
    const int Nring = ring < Nside_ ? 4 * ring : ring < 3 * Nside_ ? 4 * Nside_ : 4 * (4 * Nside_ - ring);
    const int shift = Nside_ <= ring && ring < 3 * Nside_ ? (ring - Nside_) & 1 : 0;

    int r = (pll(f) * (Nring >> 2) + i - j + 1 + shift) / 2 - 1;
    ASSERT(r < Nring);
    r = r < 0 ? r + 4 * Nside_ : r;

    return {into_unsigned(ring - 1), into_unsigned(r)};
}


size_t HEALPix::Renumber::nest_to_ring(size_t index) const {
    const auto [row, r] = nest_to_row(index);
    const auto ring     = static_cast<int>(row) + 1;

    // index of first ring pixel (ring numbering): North polar cap, equatorial belt, South polar cap
    const int N  = 4 * Nside_ - ring;
    const int r0 = ring < Nside_       ? 2 * ring * (ring - 1)
                   : ring < 3 * Nside_ ? Ncap_ + (ring - Nside_) * 4 * Nside_
                                       : Npix_ - 2 * N * (N + 1);

    return into_unsigned(r0) + r;
}


HEALPix::renumber_type HEALPix::reorder(const order_type& to) const {
    const HEALPix target(Nside_, to);

    const auto size = grid::reduced::HEALPix::size_from_nside(Nside_);
    renumber_type ren(size);

    if (order_ == to) {
        // no reordering
        std::iota(ren.begin(), ren.end(), size_t{0});
        return ren;
    }

    const auto from_nested = order_ == NESTED;
    const auto& numbering  = from_nested ? renumber() : target.renumber();

    for (size_t i = 0; i < size; ++i) {
        ren[i] = from_nested ? numbering.nest_to_ring(i) : numbering.ring_to_nest(i);
    }

    return ren;
}


}  // namespace eckit::geo::order
