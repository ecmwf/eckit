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


#include "eckit/geometry/grid/ReducedLL.h"

#include <algorithm>
#include <cmath>
#include <ostream>
#include <sstream>

#include "eckit/geometry/Domain.h"
#include "eckit/geometry/Iterator.h"
#include "eckit/geometry/Projection.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"


namespace eckit::geometry::grid {

static bool checkPl(const std::vector<long>& pl) {
    ASSERT(!pl.empty());
    return *std::min_element(pl.begin(), pl.end()) >= 2;
}

ReducedLL::ReducedLL(const Configuration& config) :
    Grid(config) {
    ASSERT(config.get("pl", pl_));
    checkPl(pl_);

    size_t Nj = 0;
    ASSERT(config.get("Nj", Nj));
    ASSERT(Nj == pl_.size());
}

ReducedLL::~ReducedLL() = default;

void ReducedLL::print(std::ostream& out) const {
    out << "ReducedLL[bbox=" << bbox() << "]";
}

size_t ReducedLL::size() const {
    size_t total = 0;
    for (const auto& j : pl_) {
        total += size_t(j);
    }
    return total;
}

bool ReducedLL::isPeriodicWestEast() const {
    ASSERT(!pl_.empty());

    auto maxpl = *std::max_element(pl_.begin(), pl_.end());
    ASSERT(maxpl >= 2);

    // if range West-East is within one increment (or greater than) 360 degree
    const Fraction inc(360, maxpl);
    return bbox().east() - bbox().west() + inc >= GLOBE;
}

bool ReducedLL::includesNorthPole() const {
    return bbox().north() == NORTH_POLE;
}

bool ReducedLL::includesSouthPole() const {
    return bbox().south() == SOUTH_POLE;
}

class ReducedLLIterator : public Iterator {

    const std::vector<long>& pl_;
    const size_t nj_;
    size_t ni_;

    const Domain domain_;
    const Fraction west_;
    const Fraction ew_;
    Fraction inc_west_east_;
    const Fraction inc_north_south_;

    Fraction latitude_;
    Fraction longitude_;

    size_t i_;
    size_t j_;
    size_t p_;

    size_t count_;
    bool first_;
    bool periodic_;

    void print(std::ostream& out) const override {
        out << "ReducedLLIterator[";
        Iterator::print(out);
        out << ",domain=" << domain_ << ",ni=" << ni_ << ",nj=" << nj_ << ",i=" << i_ << ",j=" << j_ << ",p=" << p_
            << ",count=" << count_ << "]";
    }

    bool operator++() override {
        while (j_ < nj_ && i_ < ni_) {
            // lat = latitude_;
            // lon = longitude_;

            bool contains = domain_.contains(latitude_, longitude_);
            if (contains && !first_) {
                count_++;
            }

            longitude_ += inc_west_east_;

            if (++i_ == ni_) {
                ++j_;
                i_ = 0;
                latitude_ -= inc_north_south_;
                longitude_ = west_;

                if (j_ < nj_) {
                    ASSERT(p_ < pl_.size());
                    ni_ = size_t(pl_[p_++]);
                    ASSERT(ni_ > 1);
                    inc_west_east_ = ew_ / (ni_ - (periodic_ ? 0 : 1));
                }
            }

            if (contains) {
                first_ = false;
                return true;
            }
        }
        return false;
    }

    size_t index() const override { return count_; }

    size_t size() const override { NOTIMP; }

public:
    ReducedLLIterator(const std::vector<long>& pl, const Domain& dom) :
        pl_(pl),
        nj_(pl.size()),
        domain_(dom),
        west_(domain_.west()),
        ew_((domain_.east() - domain_.west())),
        inc_north_south_(Fraction(domain_.north() - domain_.south()) / Fraction(nj_ - 1)),
        latitude_(domain_.north()),
        longitude_(west_),
        i_(0),
        j_(0),
        p_(0),
        count_(0),
        first_(true),
        periodic_(dom.isPeriodicWestEast()) {

        ASSERT(nj_ > 1);

        ni_ = size_t(pl_[p_++]);
        ASSERT(ni_ > 1);
        inc_west_east_ = ew_ / (ni_ - (periodic_ ? 0 : 1));
    }
};


static const GridRegisterType<ReducedLL> reducedLL("reduced_ll");


}  // namespace eckit::geometry::grid
