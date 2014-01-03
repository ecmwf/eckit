/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef KDTree_H
#define KDTree_H

#include "eckit/container/sptree/SPTree.h"
#include "eckit/container/kdtree/KDNode.h"

#include "KDMemory.h"
#include "KDMapped.h"


namespace eckit {

template<class Traits>
class KDTreeX : public SPTree<Traits, KDNode<Traits> > {
public:
    typedef KDNode<Traits>         Node;
    typedef SPTree<Traits,Node>    SPTreeType; // cannot redefine as SPTree since some compilers in-class redefinitions
    typedef typename Traits::Alloc Alloc;

public:
    KDTreeX(Alloc& alloc): SPTreeType(alloc) {

    }

    /// ITER must be a random access iterator
    /// WARNING: container is changed (sorted)
    template<typename ITER>
    void build(ITER begin, ITER end)
    {
        this->root_ = this->alloc_.convert(Node::build(this->alloc_, begin, end));
        this->alloc_.root(this->root_);
    }

    /// Container must be a random access
    /// WARNING: container is changed (sorted)
    template<typename Container>
    void build(Container& c)
    {
        typename Container::iterator b = c.begin();
        typename Container::iterator e = c.end();
        build(b, e);
    }

};



template<class Traits>
class KDTreeMemory : public KDTreeX< TT<Traits,KDMemory>  > {
    KDMemory alloc_;
public:
    KDTreeMemory() : KDTreeX< TT<Traits,KDMemory> >(alloc_) {}
};

template<class Traits>
class KDTreeMapped : public KDTreeX< TT<Traits,KDMapped> > {
    KDMapped alloc_;
public:
    KDTreeMapped(const eckit::PathName& path,  size_t itemCount, size_t metadataSize):
        KDTreeX< TT<Traits,KDMapped> >(alloc_),
        alloc_(path, itemCount, sizeof(KDNode<TT< Traits,KDMapped> >), metadataSize)
    {
    }

};

} // Name space


#endif
