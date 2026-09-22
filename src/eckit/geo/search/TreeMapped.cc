// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/geo/search/TreeMapped.h"


namespace eckit::geo::search {


void TreeMapped::build(std::vector<Tree::PointValueType>& v) {
    tree_.build(v);
}


void TreeMapped::insert(const Tree::PointValueType& pt) {
    tree_.insert(pt);
}


void TreeMapped::statsPrint(std::ostream& out, bool pretty) {
    tree_.statsPrint(out, pretty);
}


void TreeMapped::statsReset() {
    tree_.statsReset();
}


Tree::PointValueType TreeMapped::nearestNeighbour(const Tree::Point& pt) {
    const auto& nn = tree_.nearestNeighbour(pt).value();
    return {nn.point(), nn.payload()};
}


std::vector<Tree::PointValueType> TreeMapped::kNearestNeighbours(const Tree::Point& pt, size_t k) {
    std::vector<PointValueType> result;
    for (const auto& n : tree_.kNearestNeighbours(pt, k)) {
        result.emplace_back(PointValueType(n.point(), n.payload()));
    }
    return result;
}


std::vector<Tree::PointValueType> TreeMapped::findInSphere(const Tree::Point& pt, double radius) {
    std::vector<PointValueType> result;
    for (const auto& n : tree_.findInSphere(pt, radius)) {
        result.emplace_back(PointValueType(n.point(), n.payload()));
    }
    return result;
}


TreeMapped::TreeMapped(const Grid& r, const PathName& path) :
    Tree(r), umask_(0), path_(path), tree_(path, path.exists() ? 0 : itemCount(), 0) {}


}  // namespace eckit::geo::search
