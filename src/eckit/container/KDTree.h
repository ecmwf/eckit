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


namespace eckit {

template<class Traits>
class KDTree : public SPTree<Traits, KDNode<Traits> > {
public:
    typedef typename Traits::Alloc   Alloc;

public:
    KDTree(const Alloc& alloc = Alloc()): SPTree<Traits, KDNode<Traits> >(alloc) {}

    /// ITER must be a random access iterator
    /// WARNING: container is changed (sorted)
    template<typename ITER>
    void build(ITER begin, ITER end)
    {
        this->root_ = this->alloc_.convert(KDNode<Traits>::build(this->alloc_, begin, end));
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

} // Name space


#endif
