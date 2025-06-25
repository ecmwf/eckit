/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef KDNode_CC
#define KDNode_CC

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <algorithm>
#include <cstdio>
#include <limits>

#include "KDNode.h"

namespace eckit {


template <class Traits>
KDNode<Traits>::KDNode(const Value& value, size_t axis) : SPNodeType(value), axis_(axis) {}


template <class Traits>
void KDNode<Traits>::nearestNeighbourX(Alloc& a, const Point& p, Node*& best, double& max, int depth) {
    a.statsVisitNode();

    bool left_visited  = false;
    bool right_visited = false;

    if (p.x(axis_) < this->value_.point().x(axis_)) {
        if (this->left_) {
            this->left(a)->nearestNeighbourX(a, p, best, max, depth + 1);
            left_visited = true;
        }
    }
    else {
        if (this->right_) {
            this->right(a)->nearestNeighbourX(a, p, best, max, depth + 1);
            right_visited = true;
        }
    }

    double d = Point::distance(p, this->value_.point());

    if (d < max) {
        max  = d;
        best = this;
        a.statsNewCandidateOK();
    }
    else {
        a.statsNewCandidateMiss();
    }

    d = Point::distance(p, this->value_.point(), axis_);

    if (d < max) {

        // Visit other subtree...
        a.statsCrossOver();

        if (p.x(axis_) < this->value_.point().x(axis_)) {
            if (this->right_ && !right_visited) {
                this->right(a)->nearestNeighbourX(a, p, best, max, depth + 1);
            }
        }
        else {

            if (this->left_ && !left_visited) {
                this->left(a)->nearestNeighbourX(a, p, best, max, depth + 1);
            }
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------

template <class Traits>
void KDNode<Traits>::kNearestNeighboursX(Alloc& a, const Point& p, size_t k, NodeQueue& result, int depth) {
    if (p.x(axis_) < this->value_.point().x(axis_)) {
        if (this->left_)
            this->left(a)->kNearestNeighboursX(a, p, k, result, depth + 1);
    }
    else {
        if (this->right_)
            this->right(a)->kNearestNeighboursX(a, p, k, result, depth + 1);
    }

    double d   = Point::distance(p, this->value_.point());
    Node* self = this;
    result.push(self, a.convert(self), d);

    if (Point::distance(p, this->value_.point(), axis_) <= result.largest()) {

        // Visit other subtree...
        a.statsCrossOver();

        if (p.x(axis_) < this->value_.point().x(axis_)) {
            if (this->right_)
                this->right(a)->kNearestNeighboursX(a, p, k, result, depth + 1);
        }
        else {

            if (this->left_)
                this->left(a)->kNearestNeighboursX(a, p, k, result, depth + 1);
        }
    }
}


template <class Value>
struct sorter {
    int axis_;
    bool operator()(const Value& a, const Value& b) { return (a.point().x(axis_) < b.point().x(axis_)); }
    sorter(size_t axis) : axis_(axis) {}
};


template <class Traits>
template <typename ITER>
KDNode<Traits>* KDNode<Traits>::build(Alloc& a, const ITER& begin, const ITER& end, int depth) {
    if (end == begin)
        return 0;

    a.statsDepth(depth);

    // size_t k    = Point::size(*begin);
    size_t k    = Point::DIMS;
    size_t axis = depth % k;

    // std::sort(begin, end, sorter<Point>(axis));

    size_t median = (end - begin) / 2;

    std::nth_element(begin, begin + median, end, sorter<Value>(axis));

    ITER e2 = begin + median;
    ITER b2 = begin + median + 1;


    KDNode* n = a.newNode2(*e2, axis, (KDNode*)0);

    n->left(a, build(a, begin, e2, depth + 1));
    n->right(a, build(a, b2, end, depth + 1));

    return n;
}


template <class Traits>
KDNode<Traits>* KDNode<Traits>::insert(Alloc& a, const Value& value, KDNode<Traits>* node, int depth) {


    // size_t k    = Point::size(*begin);
    size_t k    = Point::DIMS;
    size_t axis = depth % k;


    if (node == 0) {
        return a.newNode2(value, axis, (KDNode*)0);
    }

    if (value.point().x(axis) <= node->value_.point().x(axis)) {
        node->left(a, insert(a, value, node->left(a), depth + 1));
    }
    else {
        node->right(a, insert(a, value, node->right(a), depth + 1));
    }

    return node;
}

template <class Traits>
void KDNode<Traits>::findInSphereX(Alloc& a, const Point& p, double radius, NodeList& result, int depth) {
    if (p.x(axis_) < this->value_.point().x(axis_)) {
        if (this->left_)
            this->left(a)->findInSphereX(a, p, radius, result, depth + 1);
    }
    else {
        if (this->right_)
            this->right(a)->findInSphereX(a, p, radius, result, depth + 1);
    }

    double d = Point::distance(p, this->value_.point());
    if (d <= radius) {
        result.push_back(NodeInfo(this, a.convert(this), d));
    }


    if (Point::distance(p, this->value_.point(), axis_) <= radius) {

        // Visit other subtree...

        if (p.x(axis_) < this->value_.point().x(axis_)) {
            if (this->right_)
                this->right(a)->findInSphereX(a, p, radius, result, depth + 1);
        }
        else {

            if (this->left_)
                this->left(a)->findInSphereX(a, p, radius, result, depth + 1);
        }
    }
}


}  // namespace eckit

#endif
