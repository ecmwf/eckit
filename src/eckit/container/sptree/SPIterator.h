// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef SPIterator_H
#define SPIterator_H

#include "eckit/container/sptree/SPValue.h"

namespace eckit {

template <class Traits, class NodeType>
class SPIterator {

    using Point   = typename Traits::Point;
    using Payload = typename Traits::Payload;
    using Alloc   = typename Traits::Alloc;

    using Value = SPValue<Traits>;

    using Ptr  = typename Alloc::Ptr;
    using ID   = typename Alloc::Ptr;
    using Node = NodeType;

    Alloc& alloc_;
    Ptr ptr_;


public:

    SPIterator(Alloc& alloc, Ptr ptr) : alloc_(alloc), ptr_(ptr) {
        // std::cout << "SPIterator " << ptr << std::endl;
        Node* node = alloc_.convert(ptr_, (Node*)0);
        if (node) {
            if (!node->next(alloc_)) {
                Node* prev = 0;
                node->linkNodes(alloc, prev);
            }
        }
    }

    bool operator!=(const SPIterator& other) { return ptr_ != other.ptr_; }

    operator Value*() {
        Node* n = alloc_.convert(ptr_, (Node*)0);
        return &(n->value());
    }

    Value* operator->() {
        Node* n = alloc_.convert(ptr_, (Node*)0);
        return &(n->value());
    }

    SPIterator& operator++() {
        ptr_ = alloc_.convert(ptr_, (Node*)0)->next_;
        return *this;
    }

    ID nodeID() const { return ptr_; }
};

}  // namespace eckit


#endif
