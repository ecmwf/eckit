/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef SPTree_H
#define SPTree_H

#include "eckit/container/sptree/SPIterator.h"
#include "eckit/container/sptree/SPMetadata.h"
#include "eckit/container/sptree/SPNode.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {


template <class Traits, class NodeType>
class SPTree {
public:

    typedef typename Traits::Point Point;
    typedef typename Traits::Payload Payload;
    typedef typename Traits::Alloc Alloc;

    typedef typename Alloc::Ptr Ptr;
    typedef typename Alloc::Ptr ID;

    typedef NodeType Node;
    typedef SPMetadata<Traits> Metadata;

    typedef Point PointType;
    typedef Payload PayloadType;
    typedef typename Node::NodeList NodeList;
    typedef SPNodeInfo<Traits, NodeType> NodeInfo;
    typedef typename Node::Value Value;

    Alloc& alloc_;
    Ptr root_;
    Metadata meta_;

    typedef SPIterator<Traits, NodeType> iterator;

    typedef std::pair<Point, Payload> value_type;

public:

    SPTree(Alloc& alloc) : alloc_(alloc), root_(0) {}

    ~SPTree() { alloc_.deleteNode(root_, (Node*)0); }

    void setMetadata(const Point& offset, const Point& scale) {
        meta_.offset_ = offset;
        meta_.scale_  = scale;
        alloc_.setMetadata(meta_);
    }

    NodeInfo nodeByID(ID id) { return SPNodeInfo<Traits, NodeType>(alloc_.convert(id, (Node*)0), id, 0.0); }

    void getMetadata(Point& offset, Point& scale) {
        alloc_.getMetadata(meta_);
        offset = meta_.offset_;
        scale  = meta_.scale_;
    }

    NodeInfo nearestNeighbour(const Point& p) {
        if (!root_) {
            root_ = alloc_.root();
        }
        alloc_.statsCall();
        ASSERT(root_);
        return alloc_.convert(root_, (Node*)0)->nearestNeighbour(alloc_, p);
    }

    NodeList findInSphere(const Point& p, double radius) {
        if (!root_) {
            root_ = alloc_.root();
        }
        alloc_.statsCall();
        ASSERT(root_);
        return alloc_.convert(root_, (Node*)0)->findInSphere(alloc_, p, radius);
    }

    NodeList kNearestNeighbours(const Point& p, size_t k) {
        if (!root_) {
            root_ = alloc_.root();
        }
        alloc_.statsCall();
        ASSERT(root_);
        return alloc_.convert(root_, (Node*)0)->kNearestNeighbours(alloc_, p, k);
    }

    // For testing only...
    NodeInfo nearestNeighbourBruteForce(const Point& p) {
        if (!root_) {
            root_ = alloc_.root();
        }
        alloc_.statsCall();
        ASSERT(root_);
        return alloc_.convert(root_, (Node*)0)->nearestNeighbourBruteForce(alloc_, p);
    }


    NodeList findInSphereBruteForce(const Point& p, double radius) {
        if (!root_) {
            root_ = alloc_.root();
        }
        ASSERT(root_);
        return alloc_.convert(root_, (Node*)0)->findInSphereBruteForce(alloc_, p, radius);
    }

    NodeList kNearestNeighboursBruteForce(const Point& p, size_t k) {
        if (!root_) {
            root_ = alloc_.root();
        }
        ASSERT(root_);
        return alloc_.convert(root_, (Node*)0)->kNearestNeighboursBruteForce(alloc_, p, k);
    }


    template <class Visitor>
    void visit(Visitor& v) {
        if (!root_) {
            root_ = alloc_.root();
        }
        ASSERT(root_);
        return alloc_.convert(root_, (Node*)0)->visit(alloc_, v);
    }

    void statsReset() { alloc_.statsReset(); }

    void statsPrint(std::ostream& o, bool fancy) const {
        if (fancy) {
            o << *this << ": ";
        }
        alloc_.statsPrint(o, fancy);
    }

    void print(std::ostream& o) const { o << "SPTree"; }

    friend std::ostream& operator<<(std::ostream& o, const SPTree& t) {
        t.print(o);
        return o;
    }

    iterator begin() {
        if (empty()) {
            return end();
        }
        if (!root_) {
            root_ = alloc_.root();
        }
        ASSERT(root_);
        return iterator(alloc_, root_);
    }

    iterator end() { return iterator(alloc_, 0); }

    bool empty() const { return size() == 0; }

    size_t size() const { return alloc_.nbItems(); }
};

}  // namespace eckit

#endif
