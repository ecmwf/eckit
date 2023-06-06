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


#include "eckit/geo/grid/RotatedLL.h"

#include <ostream>

#include "eckit/geo/Iterator.h"


namespace eckit::geo::grid {

RotatedLL::RotatedLL(const Configuration& config) :
    LatLon(config), rotation_(parametrisation) {}

RotatedLL::RotatedLL(const Increments& increments, const Rotation& rotation, const BoundingBox& bbox,
                     const PointLonLat& reference) :
    LatLon(increments, bbox, reference), rotation_(rotation) {}

RotatedLL::~RotatedLL() = default;

Iterator* RotatedLL::iterator() const {

    class RotatedLLIterator : protected LatLonIterator, public Iterator {
        void print(std::ostream& out) const override {
            out << "RotatedLLIterator[";
            Iterator::print(out);
            out << ",";
            LatLonIterator::print(out);
            out << "]";
        }
        bool next(double& lat, double& lon) override { return LatLonIterator::next(lat, lon); }

        size_t index() const override { return count_; }

    public:
        RotatedLLIterator(size_t ni, size_t nj, Latitude north, Longitude west, const Increments& increments,
                          const Rotation& rotation) :
            LatLonIterator(ni, nj, north, west, increments), Iterator(rotation) {}
    };

    return new RotatedLLIterator(ni_, nj_, bbox_.north(), bbox_.west(), increments_, rotation_);
}

void RotatedLL::print(std::ostream& out) const {
    out << "RotatedLL[";
    LatLon::print(out);
    out << ",rotation=" << rotation_ << "]";
}

bool RotatedLL::sameAs(const Grid& other) const {
    const auto* o = dynamic_cast<const RotatedLL*>(&other);
    return (o != nullptr) && (rotation_ == o->rotation_) && LatLon::sameAs(other);
}

const RotatedLL* RotatedLL::croppedGrid(const BoundingBox& bbox) const {
    return new RotatedLL(increments_, rotation_, bbox, reference_);
}


std::string RotatedLL::factory() const {
    return "rotated_ll";
}


static const GridBuilder<RotatedLL> rotatedLL("rotated_ll");


}  // namespace eckit::geo::grid
