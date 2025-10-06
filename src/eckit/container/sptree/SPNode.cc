/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef SPNode_CC
#define SPNode_CC

#include <algorithm>
#include <cstdio>
#include <limits>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace eckit {

template <class Traits, class NodeType>
template <class V>
SPNode<Traits, NodeType>::SPNode(const V& value) : value_(value), left_(0), right_(0), next_(0) {}

template <class Traits, class NodeType>
SPNodeInfo<Traits, NodeType> SPNode<Traits, NodeType>::nearestNeighbour(Alloc& a, const Point& p) {
    double max = Point::distance(p, value_.point());
    Node* best = this->asNode();
    asNode()->nearestNeighbourX(a, p, best, max, 0);
    return NodeInfo(best, a.convert(best), max);
}


template <class Traits, class NodeType>
SPNodeInfo<Traits, NodeType> SPNode<Traits, NodeType>::nearestNeighbourBruteForce(Alloc& a, const Point& p) {
    double max = Point::distance(p, value_.point());
    Node* best = this->asNode();
    asNode()->nearestNeighbourBruteForceX(a, p, best, max, 0);
    return NodeInfo(best, a.convert(best), max);
}


template <class Traits, class NodeType>
void SPNode<Traits, NodeType>::nearestNeighbourBruteForceX(Alloc& a, const Point& p, Node*& best, double& max,
                                                           int depth) {
    double d = Point::distance(p, value_.point());
    if (d < max) {
        best = this->asNode();
        max  = d;
    }

    if (left_)
        left(a)->nearestNeighbourBruteForceX(a, p, best, max, depth + 1);
    if (right_)
        right(a)->nearestNeighbourBruteForceX(a, p, best, max, depth + 1);
}

//----------------------------------------------------------------------------------------------------------------------


template <class Traits, class NodeType>
typename SPNode<Traits, NodeType>::NodeList SPNode<Traits, NodeType>::kNearestNeighbours(Alloc& a, const Point& p,
                                                                                         size_t k) {
    NodeQueue queue(k);
    NodeList result;
    asNode()->kNearestNeighboursX(a, p, k, queue, 0);
    queue.fill(result);
    return result;
}

template <class Traits, class NodeType>
void SPNode<Traits, NodeType>::kNearestNeighboursBruteForceX(Alloc& a, const Point& p, size_t k, NodeQueue& result,
                                                             int depth) {
    double d = Point::distance(p, value_.point());
    result.push(this->asNode(), a.convert(this->asNode()), d);
    if (left_)
        left(a)->kNearestNeighboursBruteForceX(a, p, k, result, depth + 1);
    if (right_)
        right(a)->kNearestNeighboursBruteForceX(a, p, k, result, depth + 1);
}

template <class Traits, class NodeType>
template <class Visitor>
void SPNode<Traits, NodeType>::visit(Alloc& a, Visitor& v, int depth) {
    v.enter(value_.point(), !left_ && !right_, depth);
    if (left_)
        left(a)->visit(a, v, depth + 1);
    if (right_)
        right(a)->visit(a, v, depth + 1);
    v.leave(value_.point(), !left_ && !right_, depth);
}


template <class Traits, class NodeType>
void SPNode<Traits, NodeType>::linkNodes(Alloc& a, Node*& prev) {
    if (prev) {
        prev->next(a, this->asNode());
    }
    prev = this->asNode();
    if (left_)
        left(a)->linkNodes(a, prev);
    if (right_)
        right(a)->linkNodes(a, prev);
}

template <class Traits, class NodeType>
typename SPNode<Traits, NodeType>::NodeList SPNode<Traits, NodeType>::kNearestNeighboursBruteForce(Alloc& a,
                                                                                                   const Point& p,
                                                                                                   size_t k) {
    NodeQueue queue(k);
    NodeList result;
    asNode()->kNearestNeighboursBruteForceX(a, p, k, queue, 0);
    queue.fill(result);
    return result;
}
//----------------------------------------------------------------------------------------------------------------------

template <class Traits, class NodeType>
typename SPNode<Traits, NodeType>::NodeList SPNode<Traits, NodeType>::findInSphere(Alloc& a, const Point& p,
                                                                                   double radius) {
    NodeList result;
    asNode()->findInSphereX(a, p, radius, result, 0);
    std::sort(result.begin(), result.end());
    return result;
}

template <class Traits, class NodeType>
void SPNode<Traits, NodeType>::findInSphereBruteForceX(Alloc& a, const Point& p, double radius, NodeList& result,
                                                       int depth) {
    double d = Point::distance(p, value_.point());
    if (d <= radius) {
        result.push_back(NodeInfo(this->asNode(), a.convert(this->asNode()), d));
    }
    if (left_)
        left(a)->findInSphereBruteForceX(a, p, radius, result, depth + 1);
    if (right_)
        right(a)->findInSphereBruteForceX(a, p, radius, result, depth + 1);
}

template <class Traits, class NodeType>
typename SPNode<Traits, NodeType>::NodeList SPNode<Traits, NodeType>::findInSphereBruteForce(Alloc& a, const Point& p,
                                                                                             double radius) {
    NodeList result;
    asNode()->findInSphereBruteForceX(a, p, radius, result, 0);
    std::sort(result.begin(), result.end());
    return result;
}


}  // namespace eckit

#endif
