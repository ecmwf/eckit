// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/search/TreeMapped.h"

#include <fstream>
#include <ostream>

#include "eckit/container/KDMapped.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/os/AutoUmask.h"


namespace eckit::geo::search {


void TreeMapped::build(std::vector<Value>& values) {
    ASSERT(count_ == 0);
    ASSERT(values.size() <= size());  // storage capacity

    // native value type, avoiding conversions while partitioning
    std::vector<KDTree::Value> v(values.begin(), values.end());
    tree().build(v);
    count_ = v.size();
}


void TreeMapped::insert(const Value& value) {
    ASSERT(count_ < size());  // storage capacity
    tree().insert(value);
    ++count_;
}


Neighbour TreeMapped::nearest_neighbour(const Point& p) {
    return to_neighbour(tree().nearestNeighbour(p));
}


Neighbours TreeMapped::k_nearest_neighbours(const Point& p, size_t k) {
    return to_neighbours(tree().kNearestNeighbours(p, k));
}


Neighbours TreeMapped::find_in_sphere(const Point& p, double radius) {
    return to_neighbours(tree().findInSphere(p, radius));
}


void TreeMapped::stats_print(std::ostream& out, bool pretty) const {
    tree().statsPrint(out, pretty);
}


void TreeMapped::stats_reset() {
    tree().statsReset();
}


void TreeMapped::open(const PathName& path, bool create) {
    ASSERT(!is_open());

    // shared storage, accessible to all
    AutoUmask umask(0);

    tree_  = std::make_unique<KDTree>(path, create ? size() : 0, 0);
    count_ = create ? 0 : size();
}


void TreeMapped::close() {
    tree_.reset();
    count_ = 0;
}


bool TreeMapped::valid(const PathName& path) const {
    std::ifstream in(path.localPath(), std::ios::binary);

    KDMappedHeader header(0, 0, 0);
    if (!in.read(reinterpret_cast<char*>(&header), sizeof(header))) {
        return false;
    }

    return header.headerSize_ == sizeof(KDMappedHeader) && header.itemSize_ == sizeof(KDTree::Node) &&
           header.itemCount_ == size() && header.metadataSize_ == 0 &&
           static_cast<long long>(path.size()) == static_cast<long long>(storage_size());
}


size_t TreeMapped::storage_size() const {
    constexpr size_t item = sizeof(KDTree::Node);

    // as KDMapped: header (no metadata) aligned to item size, then (unused) item 0 and items
    constexpr size_t base = ((sizeof(KDMappedHeader) + item - 1) / item) * item;
    return base + (size() + 1) * item;
}


TreeMapped::KDTree& TreeMapped::tree() {
    if (!tree_) {
        throw exception::SearchError("TreeMapped: storage not mapped", Here());
    }
    return *tree_;
}


const TreeMapped::KDTree& TreeMapped::tree() const {
    if (!tree_) {
        throw exception::SearchError("TreeMapped: storage not mapped", Here());
    }
    return *tree_;
}


}  // namespace eckit::geo::search
