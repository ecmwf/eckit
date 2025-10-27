/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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

    typedef typename Traits::Point Point;
    typedef typename Traits::Payload Payload;
    typedef typename Traits::Alloc Alloc;
    typedef SPValue<Traits> Value;
    typedef typename Alloc::Ptr ID;

    typedef NodeType Node;

    const Node* node_;
    ID id_;
    double distance_;

public:

    SPNodeInfo() : node_(0), id_(0), distance_(0) {}

    SPNodeInfo(const Node* node, ID id, double distance) : node_(node), id_(id), distance_(distance) {}

    ID id() const { return id_; }

    bool operator<(const SPNodeInfo& other) const { return distance_ < other.distance_; }

    typedef std::vector<SPNodeInfo> NodeList;

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
