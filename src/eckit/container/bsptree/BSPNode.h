/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef BSPNode_H
#define BSPNode_H

#include "BSPHyperPlane.h"

namespace eckit {

template<class Traits, class Partition>
class BSPNode : public SPNode<Traits, BSPNode<Traits, Partition> > {
public:

    typedef SPNode<Traits, BSPNode<Traits, Partition> >      SPNode;
    typedef typename SPNode::Value     Value;
    typedef typename SPNode::Alloc     Alloc;
    typedef typename SPNode::Point     Point;
    typedef typename SPNode::NodeList  NodeList;
    typedef typename SPNode::NodeQueue NodeQueue;
    typedef typename SPNode::NodeInfo  NodeInfo;

    typedef BSPHyperPlane<Point>        HyperPlane;

    typedef BSPNode<Traits, Partition> Node;

private:

    HyperPlane plane_;
    double dist_; // Distance to parent's hyperplane


public:
    BSPNode(const Value& v, const HyperPlane& plane, double dist);


    template<typename Container>
    static BSPNode* build(Alloc& a, Partition& p, const Container& nodes, double dist, int depth = 0);


private:
    virtual void nearestNeighbourX(Alloc& a,const Point& p, Node*& best, double& max, int depth);
    virtual void findInSphereX(Alloc& a,const Point& p ,double radius, NodeList& result, int depth) ;
    virtual void kNearestNeighboursX(Alloc& a,const Point& p ,size_t k, NodeQueue& result, int depth) ;

    //==========================

    template<typename Container>
    static double distanceToPlane(const Container& in, const HyperPlane& plane);

};





} // Name space

#include "BSPNode.cc"

#endif
