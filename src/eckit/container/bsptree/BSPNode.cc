/*
 * (C) Copythis->right 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef BSPNode_CC
#define BSPNode_CC

#include <random>
#include "eckit/eckit.h"

#include <cstdio>
#include <limits>
#include <random>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"


namespace eckit {
// The hyperplane is define by the vector (l, r) passing through the middle point

template <class Traits, class Partition>
BSPNode<Traits, Partition>::BSPNode(const Value& v, const HyperPlane& plane, double dist) :
    SPNodeType(v), plane_(plane), dist_(dist) {}

template <class Traits, class Partition>
void BSPNode<Traits, Partition>::nearestNeighbourX(Alloc& a, const Point& p, Node*& best, double& max, int depth) {
    a.statsVisitNode();

    if (this->left_ && this->right_) {
        // Check in which half the point lies

        double d = plane_.position(p);

        // See if we need to visit both

        double distanceToPlane = fabs(d);


        // distanceToPlane = 0;


        if (d <= 0) {
            this->left(a)->nearestNeighbourX(a, p, best, max, depth + 1);
            double dd = this->right(a)->dist_;
            if (distanceToPlane + dd <= max) {
                a.statsCrossOver();
                this->right(a)->nearestNeighbourX(a, p, best, max, depth + 1);
            }
        }
        else {

            this->right(a)->nearestNeighbourX(a, p, best, max, depth + 1);
            double dd = this->left(a)->dist_;
            if (distanceToPlane + dd <= max) {
                a.statsCrossOver();
                this->left(a)->nearestNeighbourX(a, p, best, max, depth + 1);
            }
        }
    }
    else {
        if (this->left_) {
            this->left(a)->nearestNeighbourX(a, p, best, max, depth + 1);
            return;
        }

        if (this->right_) {
            this->right(a)->nearestNeighbourX(a, p, best, max, depth + 1);
            return;
        }

        ASSERT(!this->left_ || !this->right_);

        double d = Point::distance(p, this->value_.point());

        if (d < max) {
            max  = d;
            best = this;
            a.statsNewCandidateOK();
        }
        else {
            a.statsNewCandidateMiss();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

template <class Traits, class Partition>
void BSPNode<Traits, Partition>::kNearestNeighboursX(Alloc& a, const Point& p, size_t k, NodeQueue& result, int depth) {
    if (this->left_ && this->right_) {
        // Check in which half the point lies

        double d = plane_.position(p);

        // See if we need to visit both

        double distanceToPlane = fabs(d);
        double max             = result.largest();


        if (d <= 0) {
            this->left(a)->kNearestNeighboursX(a, p, k, result, depth + 1);
            double dd = this->right(a)->dist_;
            if (result.incomplete() || distanceToPlane + dd <= max) {
                a.statsCrossOver();
                this->right(a)->kNearestNeighboursX(a, p, k, result, depth + 1);
            }
        }
        else {

            this->right(a)->kNearestNeighboursX(a, p, k, result, depth + 1);
            double dd = this->left(a)->dist_;
            if (result.incomplete() || distanceToPlane + dd <= max) {
                a.statsCrossOver();
                this->left(a)->kNearestNeighboursX(a, p, k, result, depth + 1);
            }
        }

        return;
    }


    if (this->left_) {
        this->left(a)->kNearestNeighboursX(a, p, k, result, depth + 1);
        return;
    }

    if (this->right_) {
        this->right(a)->kNearestNeighboursX(a, p, k, result, depth + 1);
        return;
    }

    // This is a leaf
    double d = Point::distance(p, this->value_.point());
    result.push(this, a.convert(this), d);
}


template <class Traits, class Partition>
template <typename Container>
double BSPNode<Traits, Partition>::distanceToPlane(const Container& in, const HyperPlane& plane) {
    double min = std::numeric_limits<double>::max();
    for (typename Container::const_iterator j = in.begin(); j != in.end(); ++j) {
        const Point& p = (*j).point();
        // Find the closest value to the partitionning plan
        double dist = fabs(plane.position(p));

        if (dist < min) {
            min = dist;
        }
    }

    return min;
}


template <class Traits, class Partition>
template <typename Container>
BSPNode<Traits, Partition>* BSPNode<Traits, Partition>::build(Alloc& a, Partition& p, const Container& nodes,
                                                              double dist, int depth) {
    HyperPlane plane;

    if (nodes.size() == 0)
        return 0;

    a.statsDepth(depth);

    if (nodes.size() == 1) {
        return a.newNode3(nodes[0], plane, dist, (BSPNode*)0);
    }


    Container left;
    Container right;


    p(nodes, left, right, plane, depth);

    if (left.size() == 0 || right.size() == 0) {
        ASSERT(left.size() == 1 || right.size() == 1);
        if (left.size() == 1) {
            return a.newNode3(left[0], plane, dist, (BSPNode*)0);
        }
        else {
            return a.newNode3(right[0], plane, dist, (BSPNode*)0);
        }
    }
    ASSERT(left.size() < nodes.size());
    ASSERT(right.size() < nodes.size());
    ASSERT(right.size() + left.size() == nodes.size());


    BSPNode* n = a.newNode3(nodes[0], plane, dist, (BSPNode*)0);


    double dl = distanceToPlane(left, plane);
    double dr = distanceToPlane(right, plane);

    // if(depth == 1) {
    // std::cerr << Partition::name() << " distanceToPlane " << dl << " " << dr << std::endl;
    //}

    n->left(a, build(a, p, left, dl, depth + 1));
    n->right(a, build(a, p, right, dr, depth + 1));

    return n;
}

template <class Traits, class Partition>
void BSPNode<Traits, Partition>::findInSphereX(Alloc& a, const Point& p, double radius, NodeList& result, int depth) {
    NOTIMP;
}


}  // namespace eckit

#endif
