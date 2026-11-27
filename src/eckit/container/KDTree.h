// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef KDTree_H
#define KDTree_H

#include "eckit/container/kdtree/KDNode.h"
#include "eckit/container/sptree/SPTree.h"

#include "KDMapped.h"
#include "KDMemory.h"


namespace eckit {

template <class Traits>
class KDTreeX : public SPTree<Traits, KDNode<Traits>> {
public:

    using Node       = KDNode<Traits>;
    using SPTreeType = SPTree<Traits, Node>;  // cannot redefine as SPTree since some compilers in-class redefinitions
    using Alloc      = typename Traits::Alloc;

    using Value   = typename SPTreeType::Value;
    using Point   = typename SPTreeType::Point;
    using Payload = typename SPTreeType::Payload;

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


// Alloc = NoStatistics<KDMemory> for queries not collecting statistics (no shared counters to write concurrently)
template <class Traits, class Alloc = KDMemory>
class KDTreeMemory : public KDTreeX<TT<Traits, Alloc>> {
    Alloc alloc_;

public:

    using KDTree  = KDTreeX<TT<Traits, Alloc>>;
    using Value   = typename KDTree::Value;
    using Point   = typename KDTree::Point;
    using Payload = typename KDTree::Payload;

public:

    KDTreeMemory() : KDTree(alloc_) {}
};

// Alloc = NoStatistics<KDMapped> for queries not collecting statistics (no shared counters to write concurrently)
template <class Traits, class Alloc = KDMapped>
class KDTreeMapped : public KDTreeX<TT<Traits, Alloc>> {
    Alloc alloc_;

public:

    using KDTree  = KDTreeX<TT<Traits, Alloc>>;
    using Value   = typename KDTree::Value;
    using Point   = typename KDTree::Point;
    using Payload = typename KDTree::Payload;
    using Node    = typename KDTree::Node;

public:

    KDTreeMapped(const eckit::PathName& path, size_t itemCount, size_t metadataSize) :
        KDTree(alloc_), alloc_(path, itemCount, sizeof(Node), metadataSize) {}
};

}  // namespace eckit


#endif
