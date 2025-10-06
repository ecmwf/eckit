/*
 * (C) Copyright 1996- ECMWF.
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

#include "eckit/container/sptree/SPNode.h"

namespace eckit {

template <class Traits, class Partition>
class BSPNode : public SPNode<Traits, BSPNode<Traits, Partition> > {
public:

    typedef SPNode<Traits, BSPNode<Traits, Partition> > SPNodeType;
    typedef typename SPNodeType::Value Value;
    typedef typename SPNodeType::Alloc Alloc;
    typedef typename SPNodeType::Point Point;
    typedef typename SPNodeType::NodeList NodeList;
    typedef typename SPNodeType::NodeQueue NodeQueue;
    typedef typename SPNodeType::NodeInfo NodeInfo;

    typedef BSPHyperPlane<Point> HyperPlane;

    typedef BSPNode<Traits, Partition> Node;

private:

    HyperPlane plane_;

    double dist_;  // Distance to parent's hyperplane

public:

    BSPNode(const Value& v, const HyperPlane& plane, double dist);
    ~BSPNode() override {}

    template <typename Container>
    static BSPNode* build(Alloc& a, Partition& p, const Container& nodes, double dist, int depth = 0);


private:

    virtual void nearestNeighbourX(Alloc& a, const Point& p, Node*& best, double& max, int depth);
    virtual void findInSphereX(Alloc& a, const Point& p, double radius, NodeList& result, int depth);
    virtual void kNearestNeighboursX(Alloc& a, const Point& p, size_t k, NodeQueue& result, int depth);

    //==========================

    template <typename Container>
    static double distanceToPlane(const Container& in, const HyperPlane& plane);
};

}  // namespace eckit

#include "BSPNode.cc"

#endif
