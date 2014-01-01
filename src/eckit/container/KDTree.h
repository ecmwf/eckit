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

#include "KDMemory.h"


namespace eckit {

template<class Traits>
class KDTree : public SPTree<Traits> {
public:
    typedef typename Traits::Alloc   Alloc;
    typedef typename SPTree<Traits>::Value   Value;
public:
    KDTree(const Alloc& alloc = Alloc()): SPTree<Traits>(alloc) {}
};

} // Name space


#endif
