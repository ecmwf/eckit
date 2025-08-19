/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef KDTree_H
#define KDTree_H

#include "eckit/container/kdtree/KDNode.h"
#include "eckit/container/sptree/SPTree.h"

#include "KDMapped.h"
#include "KDMemory.h"


namespace eckit {

template <class Traits>
class KDTreeX : public SPTree<Traits, KDNode<Traits> > {
public:

    typedef KDNode<Traits> Node;
    typedef SPTree<Traits, Node> SPTreeType;  // cannot redefine as SPTree since some compilers in-class redefinitions
    typedef typename Traits::Alloc Alloc;

    typedef typename SPTreeType::Value Value;
    typedef typename SPTreeType::Point Point;
    typedef typename SPTreeType::Payload Payload;

public:

    KDTreeX(Alloc& alloc) : SPTreeType(alloc) {}

    /// ITER must be a random access iterator
    /// WARNING: container is changed (sorted)
    template <typename ITER>
    void build(ITER begin, ITER end) {
        Alloc& a    = this->alloc_;
        this->root_ = a.convert(Node::build(a, begin, end));
        a.root(this->root_);
    }

    /// Container must be a random access
    /// WARNING: container is changed (sorted)
    template <typename Container>
    void build(Container& c) {
        typename Container::iterator b = c.begin();
        typename Container::iterator e = c.end();
        build(b, e);
    }

    //
    void insert(const Value& value) {
        Alloc& a   = this->alloc_;
        Node* root = Node::insert(a, value, a.convert(this->root_, (Node*)0), 0);

        if (a.convert(root) != this->root_) {
            a.root(a.convert(root));
            this->root_ = a.convert(root);
        }
    }
};


template <class Traits>
class KDTreeMemory : public KDTreeX<TT<Traits, KDMemory> > {
    KDMemory alloc_;

public:

    typedef KDTreeX<TT<Traits, KDMemory> > KDTree;
    typedef typename KDTree::Value Value;
    typedef typename KDTree::Point Point;
    typedef typename KDTree::Payload Payload;

public:

    KDTreeMemory() : KDTree(alloc_) {}
};

template <class Traits>
class KDTreeMapped : public KDTreeX<TT<Traits, KDMapped> > {
    KDMapped alloc_;

public:

    typedef KDTreeX<TT<Traits, KDMapped> > KDTree;
    typedef typename KDTree::Value Value;
    typedef typename KDTree::Point Point;
    typedef typename KDTree::Payload Payload;
    typedef typename KDTree::Node Node;

public:

    KDTreeMapped(const eckit::PathName& path, size_t itemCount, size_t metadataSize) :
        KDTree(alloc_), alloc_(path, itemCount, sizeof(Node), metadataSize) {}
};

}  // namespace eckit


#endif
