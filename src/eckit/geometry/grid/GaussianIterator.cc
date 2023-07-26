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


#include "eckit/geometry/grid/GaussianIterator.h"

#include <ostream>

#include "eckit/exception/Exceptions.h"


namespace eckit::geometry::grid {


GaussianIterator::GaussianIterator(const std::vector<double>& latitudes, std::vector<long>&& pl,
                                   const area::BoundingBox& bbox, size_t N, size_t Nj, size_t k) :
    latitudes_(latitudes),
    pl_(pl),
    bbox_(bbox),
    N_(N),
    Ni_(0),
    Nj_(Nj),
    i_(0),
    j_(0),
    k_(k),
    count_(0),
    first_(true) {

    // NOTE: latitudes_ span the globe, sorted from North-to-South, k_ positions the North
    // NOTE: pl is global
    ASSERT(N_ * 2 == latitudes_.size());
    ASSERT(Nj_ > 0);
}


GaussianIterator::~GaussianIterator() = default;


size_t GaussianIterator::resetToRow(size_t j) {
    ASSERT(j < latitudes_.size());
    lat_ = latitudes_[j];

    auto Ni_globe = pl_[j];
    ASSERT(Ni_globe > 1);

    inc_ = Fraction(GLOBE) / Ni_globe;

    const auto w = Fraction(bbox_.west());
    auto Nw      = (w / inc_).integralPart();
    if (Nw * inc_ < w) {
        Nw += 1;
    }

    const auto e = Fraction(bbox_.east());
    auto Ne      = (e / inc_).integralPart();
    if (Ne * inc_ > e) {
        Ne -= 1;
    }

    lon_ = Nw * inc_;
    return Nw > Ne ? 0 : std::min(size_t(Ni_globe), size_t(Ne - Nw + 1));
}


void GaussianIterator::print(std::ostream& out) const {
    out << "GaussianIterator[";
    Iterator::print(out);
    out << ",N=" << N_ << ",bbox=" << bbox_ << ",Ni=" << Ni_ << ",Nj=" << Nj_ << ",i=" << i_ << ",j=" << j_
        << ",k=" << k_ << ",count=" << count_ << "]";
}


bool GaussianIterator::operator++() {
    while (Ni_ == 0 && j_ < Nj_) {
        Ni_ = resetToRow(k_ + j_++);
    }

    if (0 < Nj_ && i_ < Ni_) {
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


size_t GaussianIterator::index() const {
    return count_;
}


size_t GaussianIterator::size() const {
    NOTIMP;
}


}  // namespace eckit::geometry::grid
