/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef KDtree_CC
#define KDTree_CC
#include "eckit/eckit.h"

#include <stdio.h>
#include <limits>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


namespace eckit {



template<class Point, class Alloc>
KDNode<Point,Alloc>::KDNode(const std::pair<const Point&,size_t>& p):
    point_(p.first),
    axis_(p.second),
    left_(0),
    right_(0),
    next_(0)
{
}

template<class Point, class Alloc>
KDNodeInfo<Point,Alloc> KDNode<Point,Alloc>::nearestNeighbour(Alloc& a,const Point& p)
{
    double max = std::numeric_limits<double>::max();
    KDNode* best = 0;
    nearestNeighbour(a, p, best, max, 0);
    return NodeInfo(best,max);
}

template<class Point, class Alloc>
void KDNode<Point,Alloc>::nearestNeighbour(Alloc& a,const Point& p, KDNode*& best, double& max, int depth)
{
    a.statsVisitNode();

    if(p.x(axis_) < point_.x(axis_))
    {
        if(left_) left(a)->nearestNeighbour(a, p, best, max, depth+1);
    }
    else
    {
        if(right_) right(a)->nearestNeighbour(a, p, best, max, depth+1);
    }

    double d   = Point::distance(p, point_);

    if(d < max) {
        max = d;
        best = this;
        a.statsNewCandidateOK();
    }
    else
    {
        a.statsNewCandidateMiss();
    }


    if(Point::distance(p, point_, axis_) < max)
    {

        // Visit other subtree...

        if(p.x(axis_) < point_.x(axis_))
        {
            if(right_) right(a)->nearestNeighbour(a, p, best, max, depth+1);

        }
        else {

            if(left_) left(a)->nearestNeighbour(a, p, best, max, depth+1);
        }
    }

}


template<class Point, class Alloc>
KDNodeInfo<Point,Alloc> KDNode<Point,Alloc>::nearestNeighbourBruteForce(Alloc& a,const Point& p)
{
    double max = std::numeric_limits<double>::max();
    KDNode* best = 0;
    nearestNeighbourBruteForce(a, p, best, max, 0);
    return NodeInfo(best,max);
}


template<class Point, class Alloc>
void KDNode<Point,Alloc>::nearestNeighbourBruteForce(Alloc& a,const Point& p, KDNode<Point,Alloc>*& best, double& max, int depth)
{
    double d = Point::distance(p,point_);
    if(d < max)
    {
        best = this;
        max  = d;
    }

    if(left_)  left(a)->nearestNeighbourBruteForce(a, p, best, max, depth+1);
    if(right_) right(a)->nearestNeighbourBruteForce(a, p, best, max, depth+1);
}

//===

template<class Point, class Alloc>
void KDNode<Point,Alloc>::kNearestNeighbours(Alloc& a,const Point& p ,size_t k, NodeQueue& result, int depth)
{
    if(p.x(axis_) < point_.x(axis_))
    {
        if(left_) left(a)->kNearestNeighbours(a, p, k, result, depth+1);
    }
    else
    {
        if(right_) right(a)->kNearestNeighbours(a, p, k, result, depth+1);
    }

    double d   = Point::distance(p, point_);
    result.push(this, d);

    if(Point::distance(p, point_, axis_) <= result.largest())
    {

        // Visit other subtree...

        if(p.x(axis_) < point_.x(axis_))
        {
            if(right_) right(a)->kNearestNeighbours(a, p,k, result, depth+1);

        }
        else {

            if(left_) left(a)->kNearestNeighbours(a, p, k, result, depth+1);
        }
    }
}


template<class Point, class Alloc>
typename KDNode<Point,Alloc>::NodeList KDNode<Point,Alloc>::kNearestNeighbours(Alloc& a,const Point& p, size_t k)
{
    NodeQueue queue(k);
    NodeList result;
    kNearestNeighbours(a,p,k,queue,0);
    queue.fill(result);
    return result;
}

template<class Point, class Alloc>
void KDNode<Point,Alloc>::kNearestNeighboursBruteForce(Alloc& a,const Point& p, size_t k, NodeQueue& result, int depth)
{
    double d = Point::distance(p,point_);
    result.push(this, d);
    if(left_)  left(a)->kNearestNeighboursBruteForce(a, p, k, result, depth+1);
    if(right_) right(a)->kNearestNeighboursBruteForce(a, p, k, result, depth+1);
}

template<class Point, class Alloc>
template<class Visitor>
void KDNode<Point,Alloc>::visit(Alloc& a,Visitor& v,int depth)
{
    v.enter(point_, !left_ && !right_, depth);
    if(left_)  left(a)->visit(a, v, depth+1);
    if(right_) right(a)->visit(a, v, depth+1);
    v.leave(point_,!left_ && !right_, depth);
}


template<class Point, class Alloc>
void KDNode<Point,Alloc>::linkNodes(Alloc& a, KDNode<Point,Alloc>*& prev)
{
    if(prev) {
        prev->next(a, this);
    }
    prev = this;
    if(left_)  left(a)->linkNodes(a,  prev);
    if(right_) right(a)->linkNodes(a, prev);
}

template<class Point, class Alloc>
typename KDNode<Point,Alloc>::NodeList KDNode<Point,Alloc>::kNearestNeighboursBruteForce(Alloc& a,const Point& p, size_t k)
{
    NodeQueue queue(k);
    NodeList result;
    kNearestNeighboursBruteForce(a,p,k,queue,0);
    queue.fill(result);
    return result;
}
//===

template<class Point>
struct sorter {
    int axis_;
    bool operator() (const Point& a,const Point& b) { return (a.x(axis_)<b.x(axis_));}
    sorter(size_t axis) : axis_(axis) {}
};


template<class Point, class Alloc>
template<typename ITER>
KDNode<Point,Alloc>* KDNode<Point,Alloc>::build(Alloc& a,
                                                const ITER& begin,
                                                const ITER& end, int depth)
{
    if(end == begin)
        return 0;

    a.statsDepth(depth);

    //size_t k    = Point::size(*begin);
    size_t k    = Point::DIMS;
    size_t axis = depth % k;

    //std::sort(begin, end, sorter<Point>(axis));

    size_t median = (end - begin)/2;

    std::nth_element(begin, begin + median, end, sorter<Point>(axis));

    ITER e2 = begin + median;
    ITER b2 = begin + median+1;


    std::pair<const Point&, size_t> p(*e2,axis);
    KDNode* n = a.newNode1(p,(KDNode*)0);

    n->left(a,build(a, begin, e2, depth + 1));
    n->right(a,build(a, b2,   end, depth + 1));

    return n;

}

template<class Point, class Alloc>
void KDNode<Point,Alloc>::findInSphere(Alloc& a,const Point& p ,double radius, NodeList& result, int depth)
{
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
}


template<class Point, class Alloc>
typename KDNode<Point,Alloc>::NodeList KDNode<Point,Alloc>::findInSphere(Alloc& a,const Point& p, double radius)
{
    NodeList result;
    findInSphere(a,p,radius,result,0);
    std::sort(result.begin(), result.end());
    return result;
}

template<class Point, class Alloc>
void KDNode<Point,Alloc>::findInSphereBruteForce(Alloc& a,const Point& p, double radius, NodeList& result, int depth)
{
    double d = Point::distance(p,point_);
    if(d <= radius) {
        result.push_back(NodeInfo(this,d));
    }
    if(left_)  left(a)->findInSphereBruteForce(a, p, radius, result, depth+1);
    if(right_) right(a)->findInSphereBruteForce(a, p, radius, result, depth+1);
}

template<class Point, class Alloc>
typename KDNode<Point,Alloc>::NodeList KDNode<Point,Alloc>::findInSphereBruteForce(Alloc& a,const Point& p, double radius)
{
    NodeList result;
    findInSphereBruteForce(a,p,radius,result,0);
    std::sort(result.begin(), result.end());
    return result;
}


} //namespace

#endif
