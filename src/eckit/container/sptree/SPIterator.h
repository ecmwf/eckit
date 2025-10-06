/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef SPIterator_H
#define SPIterator_H

#include "eckit/container/sptree/SPValue.h"

namespace eckit {

template <class Traits, class NodeType>
class SPIterator {

    typedef typename Traits::Point Point;
    typedef typename Traits::Payload Payload;
    typedef typename Traits::Alloc Alloc;

    typedef SPValue<Traits> Value;

    typedef typename Alloc::Ptr Ptr;
    typedef typename Alloc::Ptr ID;
    typedef NodeType Node;

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
