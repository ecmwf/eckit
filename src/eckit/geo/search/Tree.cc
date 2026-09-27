// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/search/Tree.h"

#include <cmath>
#include <map>
#include <sstream>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/log/Log.h"


namespace eckit::geo::search {


namespace {


class lock_type {
    inline static util::recursive_mutex MUTEX;
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


}  // namespace


Tree::Point::value_type Tree::Point::distance(const Point& p, const Point& q) {
    value_type d2 = 0;
    for (size_t i = 0; i < DIMS; ++i) {
        d2 += (p[i] - q[i]) * (p[i] - q[i]);
    }
    return std::sqrt(d2);
}

Tree::Point::value_type Tree::Point::distance(const Point& p, const Point& q, size_t axis) {
    return std::abs(p[axis] - q[axis]);
}


Tree::~Tree() = default;


Tree::Tree(const Grid& r) : itemCount_(r.size()) {
    ASSERT(itemCount_ > 0);
}


void Tree::build(std::vector<PointValueType>& /*unused*/) {
    throw exception::SeriousBug("Tree::build() not implemented for " + str());
}


void Tree::insert(const PointValueType& /*unused*/) {
    throw exception::SeriousBug("Tree::insert() not implemented for " + str());
}


void Tree::statsPrint(std::ostream& /*unused*/, bool /*unused*/) {
    throw exception::SeriousBug("Tree::statsPrint() not implemented for " + str());
}


void Tree::statsReset() {
    throw exception::SeriousBug("Tree::statsReset() not implemented for " + str());
}


Tree::PointValueType Tree::nearestNeighbour(const Point& /*unused*/) {
    throw exception::SeriousBug("Tree::nearestNeighbour() not implemented for " + str());
}


std::vector<Tree::PointValueType> Tree::kNearestNeighbours(const Point& /*unused*/, size_t /*unused*/) {
    throw exception::SeriousBug("Tree::kNearestNeighbours() not implemented for " + str());
}


std::vector<Tree::PointValueType> Tree::findInSphere(const Point& /*unused*/, double /*unused*/) {
    throw exception::SeriousBug("Tree::findInSphere() not implemented for " + str());
}


bool Tree::ready() const {
    throw exception::SeriousBug("Tree::ready() not implemented for " + str());
}


void Tree::commit() {
    throw exception::SeriousBug("Tree::commit() not implemented for " + str());
}


void Tree::print(std::ostream& out) const {
    out << "Tree[]" << std::endl;
}


void Tree::lock() {
    // Empty
}


void Tree::unlock() {
    // Empty
}


std::string Tree::str() const {
    std::ostringstream os;
    print(os);
    return os.str();
}


static std::map<std::string, TreeFactory*> TREES;


TreeFactory::TreeFactory(const std::string& name) : name_(name) {
    lock_type lock;

    if (auto j = TREES.find(name); j == TREES.end()) {
        TREES[name] = this;
        return;
    }

    throw exception::SeriousBug("TreeFactory: duplicate '" + name + "'");
}


TreeFactory::~TreeFactory() {
    lock_type lock;

    TREES.erase(name_);
}


Tree* TreeFactory::build(const std::string& name, const Grid& r) {
    lock_type lock;

    if (auto j = TREES.find(name); j != TREES.end()) {
        return j->second->make(r);
    }

    list(Log::error() << "TreeFactory: unknown '" << name << "', choices are: ");
    throw exception::SeriousBug("TreeFactory: unknown '" + name + "'");
}


void TreeFactory::list(std::ostream& out) {
    lock_type lock;

    const auto* sep = "";
    for (const auto& j : TREES) {
        out << sep << j.first;
        sep = ", ";
    }
}


}  // namespace eckit::geo::search
