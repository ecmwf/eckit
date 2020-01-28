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


#include "mir/search/tree/TreeMapped.h"


namespace mir {
namespace search {
namespace tree {


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


TreeMapped::TreeMapped(const repres::Representation& r, const eckit::PathName& path) :
    Tree(r),
    umask_(0),
    path_(path),
    tree_(path, path.exists() ? 0 : itemCount(), 0) {}


}  // namespace tree
}  // namespace search
}  // namespace mir
