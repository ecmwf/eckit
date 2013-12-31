/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef BSPtree_CC
#define BSPTree_CC

#include <random>
#include "eckit/eckit.h"

#include <stdio.h>
#include <limits>
#include <random>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"


namespace eckit {
// The hyperplane is define by the vector (l, r) passing through the middle point
template<class Point, class Payload, class Partition, class Alloc>
BSPNode<Point,Payload,Partition,Alloc>::BSPNode(const ValueType& v, const HyperPlane& plane, double dist):
    point_(v.first),
    payload_(v.second),
    plane_(plane),
    dist_(dist),
    left_(0),
    right_(0)
{
}

template<class Point, class Payload, class Partition, class Alloc>
BSPNodeInfo<Point,Payload,Partition,Alloc> BSPNode<Point,Payload,Partition,Alloc>::nearestNeighbour(Alloc& a,const Point& p)
{
    double max = std::numeric_limits<double>::max();
    BSPNode* best = 0;
    nearestNeighbour(a, p, best, max, 0);
    return NodeInfo(best, max);
}

template<class Point, class Payload, class Partition, class Alloc>
void BSPNode<Point,Payload,Partition,Alloc>::nearestNeighbour(Alloc& a,const Point& p, BSPNode*& best, double& max, int depth)
{
    a.statsVisitNode();

    if(left_ && right_) {
        // Check in which half the point lies

        double d = plane_.position(p);

        // See if we need to visit both

        double distanceToPlane = fabs(d);


        // distanceToPlane = 0;


        if(d < 0) {
            left(a)->nearestNeighbour(a, p, best, max, depth+1);
            double dd = right(a)->dist_;
            if(distanceToPlane + dd <= max) {
                right(a)->nearestNeighbour(a, p, best, max, depth+1);
            }
        }
        else {

            right(a)->nearestNeighbour(a, p, best, max, depth+1);
            double dd = left(a)->dist_;
            if(distanceToPlane + dd <= max) {
                left(a)->nearestNeighbour(a, p, best, max, depth+1);
            }
        }

    }
    else
    {
        if(left_) {
            left(a)->nearestNeighbour(a, p, best, max, depth+1);
            return;
        }

        if(right_) {
            right(a)->nearestNeighbour(a, p, best, max, depth+1);
            return;
        }

        ASSERT(!left_ || !right_);

        double d   = Point::distance(p, point_);

        if(d < max) {
            max = d;
            best = this;
            a.statsNewCandidateOK();
        }
        else {
            a.statsNewCandidateMiss();
        }

    }

}


template<class Point, class Payload, class Partition, class Alloc>
BSPNodeInfo<Point,Payload,Partition,Alloc> BSPNode<Point,Payload,Partition,Alloc>::nearestNeighbourBruteForce(Alloc& a,const Point& p)
{
    double max = std::numeric_limits<double>::max();
    BSPNode* best = 0;
    nearestNeighbourBruteForce(a, p, best, max, 0);
    return NodeInfo(best, max);
}


template<class Point, class Payload, class Partition, class Alloc>
void BSPNode<Point,Payload,Partition,Alloc>::nearestNeighbourBruteForce(Alloc& a,const Point& p, BSPNode<Point,Payload,Partition,Alloc>*& best, double& max, int depth)
{
    if(left_ || right_) {
        if(right_)
            right(a)->nearestNeighbourBruteForce(a, p, best, max, depth+1);

        if(left_)
            left(a)->nearestNeighbourBruteForce(a, p, best, max, depth+1);
    }


    double d   = Point::distance(p, point_);
    if(d < max) {
        max = d;
        best = this;
    }

}

//===

template<class Point, class Payload, class Partition, class Alloc>
void BSPNode<Point,Payload,Partition,Alloc>::kNearestNeighbours(Alloc& a,const Point& p ,size_t k, NodeQueue& result, int depth)
{
    if(left_ && right_) {
        // Check in which half the point lies

        double d =  plane_.position(p);

        // See if we need to visit both

        double distanceToPlane = fabs(d);
        double max = result.largest();


        if(d <= 0) {
            left(a)->kNearestNeighbours(a, p, k, result, depth+1);
            double dd = right(a)->dist_;
            if(result.incomplete() || distanceToPlane + dd <= max) {
                right(a)->kNearestNeighbours(a, p, k, result, depth+1);
            }
        }
        else {

            right(a)->kNearestNeighbours(a, p, k, result, depth+1);
            double dd = left(a)->dist_;
            if(result.incomplete() || distanceToPlane + dd <= max) {
                left(a)->kNearestNeighbours(a, p, k, result, depth+1);
            }
        }

        return;

    }


    if(left_) {
        left(a)->kNearestNeighbours(a, p, k, result, depth+1);
        return;

    }

    if(right_) {
        right(a)->kNearestNeighbours(a, p, k, result, depth+1);
        return;

    }

    // This is a leaf
    double d   = Point::distance(p, point_);
    result.push(this, d);

}


template<class Point, class Payload, class Partition, class Alloc>
typename BSPNode<Point,Payload,Partition,Alloc>::NodeList BSPNode<Point,Payload,Partition,Alloc>::kNearestNeighbours(Alloc& a,const Point& p, size_t k)
{
    NodeQueue queue(k);
    NodeList result;
    kNearestNeighbours(a,p,k,queue,0);
    queue.fill(result);
    return result;
}

template<class Point, class Payload, class Partition, class Alloc>
void BSPNode<Point,Payload,Partition,Alloc>::kNearestNeighboursBruteForce(Alloc& a,const Point& p, size_t k, NodeQueue& result, int depth)
{
    double d = Point::distance(p,point_);
    result.push(this, d);
    if(left_)  left(a)->kNearestNeighboursBruteForce(a, p, k, result, depth+1);
    if(right_) right(a)->kNearestNeighboursBruteForce(a, p, k, result, depth+1);
}

template<class Point, class Payload, class Partition, class Alloc>
template<class Visitor>
void BSPNode<Point,Payload,Partition,Alloc>::visit(Alloc& a,Visitor& v,int depth)
{
    v.enter(point_, !left_ && !right_, depth);
    if(left_)  left(a)->visit(a, v, depth+1);
    if(right_) right(a)->visit(a, v, depth+1);
    v.leave(point_,!left_ && !right_, depth);
}


template<class Point, class Payload, class Partition, class Alloc>
typename BSPNode<Point,Payload,Partition,Alloc>::NodeList BSPNode<Point,Payload,Partition,Alloc>::kNearestNeighboursBruteForce(Alloc& a,const Point& p, size_t k)
{
    NodeQueue queue(k);
    NodeList result;
    kNearestNeighboursBruteForce(a, p, k, queue, 0);
    queue.fill(result);
    return result;
}


template<class Point, class Payload, class Partition, class Alloc>
template<typename Container>
double BSPNode<Point,Payload,Partition,Alloc>::distanceToPlane(const Container& in, const HyperPlane& plane)
{
    double min = std::numeric_limits<double>::max();
    for(typename Container::const_iterator j = in.begin(); j != in.end(); ++j)
    {
        const Point& p = (*j);
        // Find the closest value to the partitionning plan
        double dist = fabs(plane.position(p));

        if(dist < min) {
            min = dist;
        }
    }

    return min;

}


template<class Point, class Payload, class Partition, class Alloc>
template<typename Container>
BSPNode<Point,Payload,Partition,Alloc>* BSPNode<Point,Payload,Partition,Alloc>::build(Alloc& a, Partition& p, const Container& nodes,
                                                                      double dist, int depth)
{
    HyperPlane plane;

    if(nodes.size() == 0)
        return 0;

    a.statsDepth(depth);

    if(nodes.size() == 1) {
        return a.newNode3(nodes[0], plane, dist,(BSPNode*)0);
    }


    Container  left;
    Container  right;


    p(nodes, left, right, plane, depth);

    if(left.size() == 0 || right.size() == 0) {
        ASSERT(left.size() == 1 || right.size() == 1 );
        if(left.size() == 1) {
            return a.newNode3(left[0], plane, dist, (BSPNode*)0);
        }
        else
        {
            return a.newNode3(right[0], plane, dist, (BSPNode*)0);
        }
    }
    ASSERT(left.size() < nodes.size());
    ASSERT(right.size() < nodes.size());
    ASSERT(right.size() + left.size() == nodes.size());


    BSPNode* n = a.newNode3(nodes[0], plane, dist, (BSPNode*)0);


    double dl = distanceToPlane(left, n->plane_);
    double dr = distanceToPlane(right, n->plane_);

    n->left(a, build(a, p, left, dl, depth + 1));
    n->right(a, build(a, p, right, dr, depth + 1));

    return n;

}

template<class Point, class Payload, class Partition, class Alloc>
void BSPNode<Point,Payload,Partition,Alloc>::findInSphere(Alloc& a,const Point& p ,double radius, NodeList& result, int depth)
{
    NOTIMP;
    /*
    if(p.x(axis_) < point_.x(axis_))
    {
        if(left_) left(a)->findInSphere(a, p, radius, result, depth+1);
    }
    else
    {
        if(right_) right(a)->findInSphere(a, p, radius, result, depth+1);
    }

    double d   = Point::distance(p, point_);
    if(d <= radius) {
        result.push_back(NodeInfo(this,d));
    }


    if(Point::distance(p, point_, axis_) <= radius)
    {

        // Visit other subtree...

        if(p.x(axis_) < point_.x(axis_))
        {
            if(right_) right(a)->findInSphere(a, p,radius, result, depth+1);

        }
        else {

            if(left_) left(a)->findInSphere(a, p, radius, result, depth+1);
        }
    }
    */
}


template<class Point, class Payload, class Partition, class Alloc>
typename BSPNode<Point,Payload,Partition,Alloc>::NodeList BSPNode<Point,Payload,Partition,Alloc>::findInSphere(Alloc& a,const Point& p, double radius)
{
    NodeList result;
    findInSphere(a,p,radius,result,0);
    std::sort(result.begin(), result.end());
    return result;
}

template<class Point, class Payload, class Partition, class Alloc>
void BSPNode<Point,Payload,Partition,Alloc>::findInSphereBruteForce(Alloc& a,const Point& p, double radius, NodeList& result, int depth)
{
    NOTIMP;
    /*
    double d = Point::distance(p,point_);
    if(d <= radius) {
        result.push_back(NodeInfo(this,d));
    }
    if(left_)  left(a)->findInSphereBruteForce(a, p, radius, result, depth+1);
    if(right_) right(a)->findInSphereBruteForce(a, p, radius, result, depth+1);
    */
}

template<class Point, class Payload, class Partition, class Alloc>
typename BSPNode<Point,Payload,Partition,Alloc>::NodeList BSPNode<Point,Payload,Partition,Alloc>::findInSphereBruteForce(Alloc& a,const Point& p, double radius)
{
    NodeList result;
    findInSphereBruteForce(a,p,radius,result,0);
    std::sort(result.begin(), result.end());
    return result;
}


} //namespace

#endif
