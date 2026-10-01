// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/search/TreeMemory.h"


namespace eckit::geo::search {


void TreeMemory::build(std::vector<Tree::PointValueType>& v) {
    tree_.build(v);
}


void TreeMemory::insert(const Tree::PointValueType& pt) {
    tree_.insert(pt);
}


void TreeMemory::statsPrint(std::ostream& out, bool pretty) {
    tree_.statsPrint(out, pretty);
}


void TreeMemory::statsReset() {
    tree_.statsReset();
}


Tree::PointValueType TreeMemory::nearestNeighbour(const Tree::Point& pt) {
    const auto& nn = tree_.nearestNeighbour(pt).value();
    return {nn.point(), nn.payload()};
}


std::vector<Tree::PointValueType> TreeMemory::kNearestNeighbours(const Tree::Point& pt, size_t k) {
    std::vector<PointValueType> result;
    for (const auto& n : tree_.kNearestNeighbours(pt, k)) {
        result.emplace_back(PointValueType(n.point(), n.payload()));
    }
    return result;
}


std::vector<Tree::PointValueType> TreeMemory::findInSphere(const Tree::Point& pt, double radius) {
    std::vector<PointValueType> result;
    for (const auto& n : tree_.findInSphere(pt, radius)) {
        result.emplace_back(PointValueType(n.point(), n.payload()));
    }
    return result;
}


bool TreeMemory::ready() const {
    return false;
}


void TreeMemory::commit() {}


void TreeMemory::print(std::ostream& out) const {
    out << "TreeMemory[]";
}


static const TreeBuilder<TreeMemory> builder("memory");


}  // namespace eckit::geo::search
