/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef SPNodeQueue_H
#define SPNodeQueue_H

#include <algorithm>
#include <limits>
#include <queue>


namespace eckit {


template <class Traits, class NodeType>
class SPNode;

template <class Traits, class NodeType>
class SPIterator;

template <class Traits>
class SPValue;

template <class Traits, class NodeType>
struct SPNodeInfo;

template <class Traits, class NodeType>
class SPNodeQueue {
public:

    typedef typename Traits::Point Point;
    typedef typename Traits::Payload Payload;
    typedef typename Traits::Alloc Alloc;

    typedef typename Alloc::Ptr ID;

    typedef NodeType Node;
    typedef SPNodeInfo<Traits, NodeType> NodeInfo;
    typedef typename NodeInfo::NodeList NodeList;

private:

    size_t k_;
    std::priority_queue<NodeInfo> queue_;

public:

    SPNodeQueue(size_t k) : k_(k) {}

    void push(Node* n, ID id, double d) {
        NodeInfo info(n, id, d);
        queue_.push(info);
        while (queue_.size() > k_) {
            queue_.pop();
        }
    }

    double largest() const { return queue_.size() ? queue_.top().distance_ : std::numeric_limits<double>::max(); }

    void fill(NodeList& v) {
        v.reserve(k_);
        while (!queue_.empty()) {
            v.push_back(queue_.top());
            queue_.pop();
        }
        std::sort(v.begin(), v.end());
    }

    bool incomplete() const { return queue_.size() < k_; }
};

}  // namespace eckit


#endif
