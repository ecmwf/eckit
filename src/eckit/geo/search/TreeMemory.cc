// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/search/TreeMemory.h"

#include <ostream>

#include "eckit/geo/Exceptions.h"


namespace eckit::geo::search {


static const TreeRegisterType<TreeMemory> BUILDER("memory");


TreeMemory::TreeMemory(const std::string& uid, size_t size) : Tree(uid, size) {}


void TreeMemory::build(std::vector<Value>& values) {
    ASSERT(count_ == 0);
    ASSERT(values.size() <= size());

    // native value type, avoiding conversions while partitioning
    std::vector<KDTree::Value> v(values.begin(), values.end());
    tree_.build(v);
    count_ = v.size();
}


void TreeMemory::insert(const Value& value) {
    ASSERT(count_ < size());
    tree_.insert(value);
    ++count_;
}


Neighbour TreeMemory::nearest_neighbour(const Point& p) {
    return to_neighbour(tree_.nearestNeighbour(p));
}


Neighbours TreeMemory::k_nearest_neighbours(const Point& p, size_t k) {
    return to_neighbours(tree_.kNearestNeighbours(p, k));
}


Neighbours TreeMemory::find_in_sphere(const Point& p, double radius) {
    return to_neighbours(tree_.findInSphere(p, radius));
}


void TreeMemory::stats_print(std::ostream& out, bool pretty) const {
    tree_.statsPrint(out, pretty);
}


void TreeMemory::stats_reset() {
    tree_.statsReset();
}


void TreeMemory::print(std::ostream& out) const {
    out << "TreeMemory[size=" << size() << "]";
}


}  // namespace eckit::geo::search
