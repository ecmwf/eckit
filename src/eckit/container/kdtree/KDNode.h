// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef KDNode_H
#define KDNode_H

#include "eckit/container/sptree/SPNode.h"

namespace eckit {


template <class Traits>
class KDNode : public SPNode<Traits, KDNode<Traits>> {
public:

    using SPNodeType = SPNode<Traits, KDNode<Traits>>;
    // cannot redefine as SPNode since some compilers in-class redefinitions

    using Value     = typename SPNodeType::Value;
    using Alloc     = typename SPNodeType::Alloc;
    using Point     = typename SPNodeType::Point;
    using NodeList  = typename SPNodeType::NodeList;
    using NodeQueue = typename SPNodeType::NodeQueue;
    using NodeInfo  = typename SPNodeType::NodeInfo;


    using Node = KDNode<Traits>;

private:

    size_t axis_;

public:

    KDNode(const Value& value, size_t axis);
    ~KDNode() {}

    template <typename ITER>
    static KDNode* build(Alloc& a, const ITER& begin, const ITER& end, int depth = 0);

    static KDNode<Traits>* insert(Alloc& a, const Value& value, KDNode<Traits>* node, int depth = 0);

    /// Return the axis along which this node is split.
    size_t axis() const { return axis_; }

public:

    void nearestNeighbourX(Alloc& a, const Point& p, Node*& best, double& max, int depth);
    void findInSphereX(Alloc& a, const Point& p, double radius, NodeList& result, int depth);
    void kNearestNeighboursX(Alloc& a, const Point& p, size_t k, NodeQueue& result, int depth);

    //==========================
};


}  // namespace eckit

#include "KDNode.cc"

#endif
