// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef BSPTree_H
#define BSPTree_H

#include "eckit/container/KDMapped.h"
#include "eckit/container/KDMemory.h"
#include "eckit/container/bsptree/BSPNode.h"
#include "eckit/container/sptree/SPTree.h"

namespace eckit {

template <class Traits, class Partition>
class BSPTreeX : public SPTree<Traits, BSPNode<Traits, Partition> > {
public:

    using Alloc = typename Traits::Alloc;

private:

    Partition partition_;

public:

    BSPTreeX(Alloc& alloc) : SPTree<Traits, BSPNode<Traits, Partition> >(alloc) {}


    /// Container must be a random access
    /// WARNING: container is changed (sorted)
    template <typename Container>
    void build(Container& nodes) {
        this->root_ = this->alloc_.convert(BSPNode<Traits, Partition>::build(this->alloc_, partition_, nodes, 0.0));
        this->alloc_.root(this->root_);
    }
};

template <class Traits, class Partition>
class BSPTreeMemory : public BSPTreeX<TT<Traits, KDMemory>, Partition> {
    KDMemory alloc_;

public:

    BSPTreeMemory() : BSPTreeX<TT<Traits, KDMemory>, Partition>(alloc_) {}
};

template <class Traits, class Partition>
class BSPTreeMapped : public BSPTreeX<TT<Traits, KDMapped>, Partition> {
    KDMapped alloc_;

public:

    BSPTreeMapped(const eckit::PathName& path, size_t itemCount, size_t metadataSize) :
        BSPTreeX<TT<Traits, KDMapped>, Partition>(alloc_),
        alloc_(path, itemCount, sizeof(BSPNode<TT<Traits, KDMapped>, Partition>), metadataSize) {}
};


}  // namespace eckit


#endif
