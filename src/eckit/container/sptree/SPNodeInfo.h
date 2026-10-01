// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef SPNodeInfo_H
#define SPNodeInfo_H

#include <iostream>
#include <vector>

namespace eckit {


template <class Traits, class NodeType>
class SPNode;

template <class Traits>
class SPTreeIterator;

template <class Traits>
class SPValue;

template <class Traits, class NodeType>
struct SPNodeInfo {

    using Point   = typename Traits::Point;
    using Payload = typename Traits::Payload;
    using Alloc   = typename Traits::Alloc;
    using Value   = SPValue<Traits>;
    using ID      = typename Alloc::Ptr;

    using Node = NodeType;

    const Node* node_;
    ID id_;
    double distance_;

public:

    SPNodeInfo() : node_(0), id_(0), distance_(0) {}

    SPNodeInfo(const Node* node, ID id, double distance) : node_(node), id_(id), distance_(distance) {}

    ID id() const { return id_; }

    bool operator<(const SPNodeInfo& other) const { return distance_ < other.distance_; }

    using NodeList = std::vector<SPNodeInfo>;

    const Point& point() const { return node_->point(); }
    const Payload& payload() const { return node_->payload(); }
    const Value& value() const { return node_->value(); }

    double distance() const { return distance_; }

    friend std::ostream& operator<<(std::ostream& s, const SPNodeInfo& p) {
        s << "[value=" << p.value() << ",distance=" << p.distance() << "]";
        return s;
    }
};


}  // namespace eckit


#endif
