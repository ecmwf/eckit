/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef BSPTree_H
#define BSPTree_H

#include "eckit/container/sptree/SPTree.h"
#include "eckit/container/bsptree/BSPNode.h"

#include "KDMemory.h"
#include "KDMapped.h"

namespace eckit {

template<class Traits>
class BSPTreeX : public SPTree<Traits, BSPNode<Traits> > {
public:
    typedef typename Traits::Alloc     Alloc;
    typedef typename Traits::Partition Partition;

private:

    Partition partition_;

public:
    BSPTreeX(Alloc& alloc): SPTree<Traits, BSPNode<Traits> >(alloc) {}


    /// Container must be a random access
    /// WARNING: container is changed (sorted)
    template<typename Container>
    void build(Container& nodes)
    {
        this->root_ = this->alloc_.convert(BSPNode<Traits>::build(this->alloc_, partition_, nodes, 0.0));
        this->alloc_.root(this->root_);
    }

};

template<class Traits>
class BSPTreeMemory : public BSPTreeX< TT<Traits,KDMemory>  > {
    KDMemory alloc_;
public:
    BSPTreeMemory() : BSPTreeX< TT<Traits,KDMemory> >(alloc_) {}
};

template<class Traits>
class BSPTreeMapped : public BSPTreeX< TT<Traits,KDMapped> > {
    KDMapped alloc_;
public:
    BSPTreeMapped(const eckit::PathName& path,  size_t itemCount, size_t metadataSize):
        BSPTreeX< TT<Traits,KDMapped> >(alloc_),
        alloc_(path, itemCount, sizeof(BSPNode<TT< Traits,KDMapped> >), metadataSize)
    {
    }

};


} // Name space


#endif
