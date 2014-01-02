/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef SPTree_H
#define SPTree_H

#include "eckit/container/sptree/SPNode.h"
#include "eckit/container/sptree/SPMetadata.h"
#include "eckit/container/sptree/SPIterator.h"

namespace eckit {


template<class Traits, class NodeType>
class SPTree {

public:
    typedef typename Traits::Point   Point;
    typedef typename Traits::Payload Payload;
    typedef typename Traits::Alloc   Alloc;

    typedef typename Alloc::Ptr Ptr;
    typedef typename Alloc::Ptr ID;

    typedef NodeType Node;
    typedef SPMetadata<Traits> Metadata;

    typedef          Point   PointType;
    typedef          Payload PayloadType;
    typedef typename Node::NodeList NodeList;
    typedef          SPNodeInfo<Traits>       NodeInfo;
    typedef typename Node::Value Value;

    Alloc alloc_;
    Ptr   root_;
    Metadata meta_;

    typedef SPIterator<Traits> iterator;

    typedef std::pair<Point,Payload> value_type;

public:

    SPTree(const Alloc& alloc = Alloc()): alloc_(alloc), root_(alloc.root()) {}

    ~SPTree() {
        alloc_.deleteNode(root_,(Node*)0);
    }

    void setMetadata(const Point& offset, const Point& scale) {
        meta_.offset_ = offset;
        meta_.scale_  = scale;
        alloc_.setMetadata(meta_);
    }

    NodeInfo nodeByID(ID id) {
        return SPNodeInfo<Traits>(alloc_.convert(id,(Node*)0), id, 0.0);
    }

    void getMetadata(Point& offset, Point& scale) {
        alloc_.getMetadata(meta_);
        offset = meta_.offset_;
        scale  = meta_.scale_;
    }



    NodeInfo nearestNeighbour(const Point& p)
    {
        alloc_.statsCall();
        return alloc_.convert(root_,(Node*)0)->nearestNeighbour(alloc_, p);
    }

    NodeList findInSphere(const Point& p,double radius)
    {
        alloc_.statsCall();
        return alloc_.convert(root_,(Node*)0)->findInSphere(alloc_, p, radius);
    }

    NodeList kNearestNeighbours(const Point& p, size_t k)
    {
        alloc_.statsCall();
        return alloc_.convert(root_,(Node*)0)->kNearestNeighbours(alloc_, p, k);
    }

    // For testing only...
    NodeInfo nearestNeighbourBruteForce(const Point& p)
    {
        alloc_.statsCall();
        return alloc_.convert(root_,(Node*)0)->nearestNeighbourBruteForce(alloc_, p);
    }


    NodeList findInSphereBruteForce(const Point& p,double radius)
    {
        return alloc_.convert(root_,(Node*)0)->findInSphereBruteForce(alloc_, p, radius);
    }

    NodeList kNearestNeighboursBruteForce(const Point& p,size_t k)
    {
        return alloc_.convert(root_,(Node*)0)->kNearestNeighboursBruteForce(alloc_, p, k);
    }


    template<class Visitor>
    void visit(Visitor& v)
    {
        return alloc_.convert(root_,(Node*)0)->visit(alloc_, v);
    }

    void statsReset() {
        alloc_.statsReset();
    }

    void statsPrint(std::ostream& o, bool fancy) const {
        if(fancy) o << *this << ": ";
        alloc_.statsPrint(o, fancy);
    }

    void print(std::ostream& o) const {
        o << "SPTree";
    }

    friend std::ostream& operator<<(std::ostream& o, const SPTree& t) {
        t.print(o);
        return o;
    }

    iterator begin() {
        return iterator(alloc_, root_);
    }

    iterator end() {
        return iterator(alloc_, 0);
    }
};

} // Name space

//#include "SPTree.cc"

#endif
