// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/search/KDTreeMemory.h"

#include <ostream>

#include "eckit/geo/Exceptions.h"


namespace eckit::geo::search {


KDTreeMemory::KDTreeMemory(size_t size) : KDTree(size) {}


bool KDTreeMemory::build(const Fill& fill, bool balanced) {
    if (count_ > 0) {
        return false;
    }

    auto values = this->values(fill);

    if (balanced) {
        // native value type, avoiding conversions while partitioning
        std::vector<Container::Value> v(values.begin(), values.end());
        tree_.build(v);
    }
    else {
        for (const auto& value : values) {
            tree_.insert(value);
        }
    }

    count_ = values.size();
    return true;
}


Neighbour KDTreeMemory::nearest_neighbour(const Point& p) {
    return to_neighbour(tree_.nearestNeighbour(p));
}


Neighbours KDTreeMemory::k_nearest_neighbours(const Point& p, size_t k) {
    return to_neighbours(tree_.kNearestNeighbours(p, k));
}


Neighbours KDTreeMemory::find_in_sphere(const Point& p, double radius) {
    return to_neighbours(tree_.findInSphere(p, radius));
}


void KDTreeMemory::stats_print(std::ostream& out, bool pretty) const {
    tree_.statsPrint(out, pretty);
}


void KDTreeMemory::stats_reset() {
    tree_.statsReset();
}


void KDTreeMemory::print(std::ostream& out) const {
    out << "KDTreeMemory[size=" << size() << "]";
}


}  // namespace eckit::geo::search
