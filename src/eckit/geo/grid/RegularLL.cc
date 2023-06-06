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


#include "eckit/geo/grid/RegularLL.h"

#include <ostream>

#include "eckit/geo/Iterator.h"


namespace eckit::geo::grid {

RegularLL::RegularLL(const Configuration& config) :
    LatLon(config) {}

RegularLL::RegularLL(const Increments& increments, const BoundingBox& bbox, const PointLonLat& reference) :
    LatLon(increments, bbox, reference) {}

RegularLL::~RegularLL() = default;

Iterator* RegularLL::iterator() const {

    class RegularLLIterator : protected LatLonIterator, public Iterator {
        void print(std::ostream& out) const override {
            out << "RegularLLIterator[";
            Iterator::print(out);
            out << ",";
            LatLonIterator::print(out);
            out << "]";
        }
        bool next(double& lat, double& lon) override { return LatLonIterator::next(lat, lon); }

        size_t index() const override { return count_; }

    public:
        RegularLLIterator(size_t ni, size_t nj, Latitude north, Longitude west, const Increments& increments) :
            LatLonIterator(ni, nj, north, west, increments) {}
    };

    return new RegularLLIterator(ni_, nj_, bbox_.north(), bbox_.west(), increments_);
}

void RegularLL::print(std::ostream& out) const {
    out << "RegularLL[";
    LatLon::print(out);
    out << "]";
}

bool RegularLL::sameAs(const Grid& other) const {
    const auto* o = dynamic_cast<const RegularLL*>(&other);
    return (o != nullptr) && LatLon::sameAs(other);
}

const RegularLL* RegularLL::croppedGrid(const BoundingBox& bbox) const {
    return new RegularLL(increments_, bbox, reference_);
}

std::string RegularLL::factory() const {
    return "regular_ll";
}


static const GridBuilder<RegularLL> regularLL("regular_ll");


}  // namespace eckit::geo::grid
