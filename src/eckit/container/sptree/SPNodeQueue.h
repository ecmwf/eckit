// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef SPNodeQueue_H
#define SPNodeQueue_H

#include <algorithm>
#include <limits>
#include <utility>
#include <vector>


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

    using Point   = typename Traits::Point;
    using Payload = typename Traits::Payload;
    using Alloc   = typename Traits::Alloc;

    using ID = typename Alloc::Ptr;

    using Node     = NodeType;
    using NodeInfo = SPNodeInfo<Traits, NodeType>;
    using NodeList = typename NodeInfo::NodeList;

private:

    size_t k_;
    std::vector<NodeInfo> heap_;  // max-heap by distance, at most k

    // the farthest replaced by a nearer one (a single sift down, rather than a push and a pop)
    void replace_top(const NodeInfo& info) {
        const auto n = heap_.size();
        size_t i     = 0;
        for (size_t c = 1; c < n; c = 2 * i + 1) {
            if (c + 1 < n && heap_[c] < heap_[c + 1]) {
                ++c;
            }
            if (!(info < heap_[c])) {
                break;
            }
            heap_[i] = heap_[c];
            i        = c;
        }
        heap_[i] = info;
    }

public:

    SPNodeQueue(size_t k) : k_(k) { heap_.reserve(k); }

    void push(Node* n, ID id, double d) {
        if (heap_.size() < k_) {
            heap_.emplace_back(n, id, d);
            std::push_heap(heap_.begin(), heap_.end());
        }
        else if (k_ > 0 && d < heap_.front().distance_) {
            replace_top(NodeInfo(n, id, d));
        }
    }

    double largest() const { return heap_.empty() ? std::numeric_limits<double>::max() : heap_.front().distance_; }

    // the candidates, nearest first (the queue is left empty)
    void fill(NodeList& v) {
        v = std::move(heap_);
        heap_.clear();
        std::sort(v.begin(), v.end());
    }

    bool incomplete() const { return heap_.size() < k_; }
};

}  // namespace eckit


#endif
