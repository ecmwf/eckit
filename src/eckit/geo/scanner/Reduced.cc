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


#include "eckit/geo/scanner/Reduced.h"

#include <ostream>

#include "eckit/exception/Exceptions.h"


namespace eckit::geo::scanner {


Reduced::Reduced(const std::vector<double>& latitudes,
                 pl_type&& pl,
                 const BoundingBox& bbox,
                 size_t N,
                 size_t Nj,
                 size_t k) :
    latitudes_(latitudes), pl_(pl), bbox_(bbox), N_(N), Ni_(0), Nj_(Nj), lat_(latitudes[k]), i_(0), j_(0), k_(k), count_(0), first_(true), p_(PointLonLat{0, 0}) {
    // NOTE: latitudes_ span the globe, sorted from North-to-South, k_ positions the North
    // NOTE: pl is global
    ASSERT(N_ * 2 == latitudes_.size());
    ASSERT(Nj_ > 0);
}


void Reduced::print(std::ostream& out) const {
    out << "Reduced[N=" << N_ << /*",bbox=" << bbox_ <<*/ ",Ni=" << Ni_ << ",Nj=" << Nj_
        << ",i=" << i_ << ",j=" << j_ << ",k=" << k_ << ",count=" << count_ << "]";
}


size_t Reduced::index() const {
    return count_;
}


size_t Reduced::resetToRow(size_t j) {
    ASSERT(j < latitudes_.size());
    lat_ = latitudes_[j];

    auto Ni_globe = static_cast<Fraction::value_type>(pl_[j]);
    ASSERT(Ni_globe > 1);

    inc_ = {360, Ni_globe};

    const auto w = Fraction{bbox_.west()};
    auto Nw      = (w / inc_).integralPart();
    if (Nw * inc_ < w) {
        Nw += 1;
    }

    const auto e = Fraction{bbox_.east()};
    auto Ne      = (e / inc_).integralPart();
    if (Ne * inc_ > e) {
        Ne -= 1;
    }

    lon_ = Nw * inc_;
    return Nw > Ne ? 0 : std::min(static_cast<size_t>(Ni_globe), static_cast<size_t>(Ne - Nw + 1));
}


bool Reduced::operator++() {
    while (Ni_ == 0 && j_ < Nj_) {
        Ni_ = resetToRow(k_ + j_++);
    }

    if (0 < Nj_ && i_ < Ni_) {
        p_ = PointLonLat{lon_, lat_};

        lon_ += inc_;

        if (first_) {
            first_ = false;
        }
        else {
            count_++;
        }

        if (++i_ == Ni_) {
            i_  = 0;
            Ni_ = 0;
        }

        return true;
    }
    return false;
}


size_t Reduced::size() const {
    return 0;
}


}  // namespace eckit::geo::scanner
