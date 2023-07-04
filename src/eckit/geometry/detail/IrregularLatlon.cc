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


#include "eckit/geometry/detail/IrregularLatlon.h"

#include <cmath>
#include <ostream>

#include "eckit/utils/MD5.h"

#include "eckit/geometry/Iterator.h"
#include "eckit/geometry/Projection.h"


namespace eckit::geometry::detail {


static void range(const std::vector<double>& v, double& mn, double& mx, double& dmax) {
    ASSERT(v.size() >= 2);

    dmax = 0;
    mx   = v[0];
    mn   = v[0];

    for (size_t i = 1; i < v.size(); ++i) {
        double d = std::abs(v[i] - v[i - 1]);
        dmax     = std::max(d, dmax);
        mx       = std::max(v[i], mx);
        mn       = std::min(v[i], mn);
    }
}


IrregularLatlon::IrregularLatlon(const Configuration& config) :
    Grid(config) {
    ASSERT(config.get("latitudes", latitudes_));
    range(latitudes_, south_, north_, south_north_);

    ASSERT(config.get("longitudes", longitudes_));
    range(longitudes_, west_, east_, west_east_);
}


size_t IrregularLatlon::numberOfPoints() const {
    return latitudes_.size() * longitudes_.size();
}


void IrregularLatlon::print(std::ostream& out) const {
    out << "IrregularLatlon[latitudes=" << latitudes_.size() << ",longitudes=" << longitudes_.size() << "]";
}


class IrregularLatlonIterator : public Iterator {
    size_t i_;
    size_t ni_;
    size_t j_;
    size_t nj_;
    size_t count_;
    bool first_;

    const std::vector<double>& latitudes_;
    const std::vector<double>& longitudes_;

    void print(std::ostream& out) const override {
        out << "IrregularLatlonIterator[";
        Iterator::print(out);
        out << "]";
    }

    bool operator++() override {
        if (j_ < nj_) {
            if (i_ < ni_) {
                // lat = latitudes_[j_];
                // lon = longitudes_[i_];

                if (first_) {
                    first_ = false;
                }
                else {
                    count_++;
                }

                if (++i_ == ni_) {
                    ++j_;
                    i_ = 0;
                }

                return true;
            }
        }
        return false;
    }

    size_t index() const override { return count_; }

    size_t size() const override { NOTIMP; }

public:
    // TODO: Consider keeping a reference on the latitudes and bbox, to avoid copying

    IrregularLatlonIterator(const std::vector<double>& latitudes, const std::vector<double>& longitudes) :
        i_(0),
        ni_(longitudes.size()),
        j_(0),
        nj_(latitudes.size()),
        count_(0),
        first_(true),
        latitudes_(latitudes),
        longitudes_(longitudes) {}

    ~IrregularLatlonIterator() override {
        auto count = count_ + (i_ > 0 || j_ > 0 ? 1 : 0);
        ASSERT(count == ni_ * nj_);
    }

    IrregularLatlonIterator(const IrregularLatlonIterator&)            = delete;
    IrregularLatlonIterator(IrregularLatlonIterator&&)                 = delete;
    IrregularLatlonIterator& operator=(const IrregularLatlonIterator&) = delete;
    IrregularLatlonIterator& operator=(IrregularLatlonIterator&&)      = delete;
};


Iterator* IrregularLatlon::iterator() const {
    return new IrregularLatlonIterator(latitudes_, longitudes_);
}


bool IrregularLatlon::isPeriodicWestEast() const {
    return (east_ - west_) + west_east_ >= GLOBE;
}


bool IrregularLatlon::includesNorthPole() const {
    return north_ + south_north_ >= NORTH_POLE;
}


bool IrregularLatlon::includesSouthPole() const {
    return south_ - south_north_ <= SOUTH_POLE;
}


Renumber IrregularLatlon::crop(BoundingBox&) const {
    NOTIMP;
}


Renumber IrregularLatlon::reorder(long scanningMode) const {
    NOTIMP;
}


static const GridBuilder<IrregularLatlon> irregularLatlon("irregular_latlon");


}  // namespace eckit::geometry::detail
