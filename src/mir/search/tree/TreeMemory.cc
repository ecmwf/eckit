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


#include "mir/search/tree/TreeMemory.h"


namespace mir {
namespace search {
namespace tree {


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


TreeMemory::TreeMemory(const repres::Representation& r, const param::MIRParametrisation&) : Tree(r) {}


static TreeBuilder<TreeMemory> builder("memory");


}  // namespace tree
}  // namespace search
}  // namespace mir
