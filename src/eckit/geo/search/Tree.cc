// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/search/Tree.h"

#include <ostream>

#include "eckit/geo/Exceptions.h"


namespace eckit::geo::search {


std::ostream& operator<<(std::ostream& out, const Neighbour& n) {
    return out << "Neighbour[point=" << n.point << ",index=" << n.index << ",distance=" << n.distance << "]";
}


Tree::Tree(const std::string& uid, size_t size) : uid_(uid), size_(size) {
    if (size_ == 0) {
        throw exception::SearchError("Tree: cannot build tree of 0 points", Here());
    }
}


Tree::~Tree() = default;


Tree* TreeFactory::build(const std::string& type, const std::string& uid, size_t size) {
    if (!has_type(type)) {
        throw exception::SearchError("TreeFactory: unknown '" + type + "'", Here());
    }

    return Factory<Tree>::instance().get(type).create(uid, size);
}


std::ostream& TreeFactory::list(std::ostream& out) {
    return out << Factory<Tree>::instance() << std::endl;
}


}  // namespace eckit::geo::search
