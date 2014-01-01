/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef SPNode_CC
#define SPNode_CC
#include "eckit/eckit.h"

#include <stdio.h>
#include <limits>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


namespace eckit {



template<class Traits>
SPNode<Traits>::SPNode(const Value& value, size_t axis):
    value_(value),
    axis_(axis),
    left_(0),
    right_(0),
    next_(0)
{
}

template<class Traits>
SPNodeInfo<Traits> SPNode<Traits>::nearestNeighbour(Alloc& a,const Point& p)
{
    double max = std::numeric_limits<double>::max();
    SPNode* best = 0;
    nearestNeighbour(a, p, best, max, 0);
    return NodeInfo(best,a.convert(best), max);
}

template<class Traits>
void SPNode<Traits>::nearestNeighbour(Alloc& a,const Point& p, SPNode*& best, double& max, int depth)
{
    a.statsVisitNode();

    if(p.x(axis_) < value_.point().x(axis_))
    {
        if(left_) left(a)->nearestNeighbour(a, p, best, max, depth+1);
    }
    else
    {
        if(right_) right(a)->nearestNeighbour(a, p, best, max, depth+1);
    }

    double d   = Point::distance(p, value_.point());

    if(d < max) {
        max = d;
        best = this;
        a.statsNewCandidateOK();
    }
    else
    {
        a.statsNewCandidateMiss();
    }


    if(Point::distance(p, value_.point(), axis_) < max)
    {

        // Visit other subtree...

        if(p.x(axis_) < value_.point().x(axis_))
        {
            if(right_) right(a)->nearestNeighbour(a, p, best, max, depth+1);

        }
        else {

            if(left_) left(a)->nearestNeighbour(a, p, best, max, depth+1);
        }
    }

}


template<class Traits>
SPNodeInfo<Traits> SPNode<Traits>::nearestNeighbourBruteForce(Alloc& a,const Point& p)
{
    double max = std::numeric_limits<double>::max();
    SPNode* best = 0;
    nearestNeighbourBruteForce(a, p, best, max, 0);
    return NodeInfo(best,max);
}


template<class Traits>
void SPNode<Traits>::nearestNeighbourBruteForce(Alloc& a,const Point& p, SPNode<Traits>*& best, double& max, int depth)
{
    double d = Point::distance(p,value_.point());
    if(d < max)
    {
        best = this;
        max  = d;
    }

    if(left_)  left(a)->nearestNeighbourBruteForce(a, p, best, max, depth+1);
    if(right_) right(a)->nearestNeighbourBruteForce(a, p, best, max, depth+1);
}

//===

template<class Traits>
void SPNode<Traits>::kNearestNeighbours(Alloc& a,const Point& p ,size_t k, NodeQueue& result, int depth)
{
    if(p.x(axis_) < value_.point().x(axis_))
    {
        if(left_) left(a)->kNearestNeighbours(a, p, k, result, depth+1);
    }
    else
    {
        if(right_) right(a)->kNearestNeighbours(a, p, k, result, depth+1);
    }

    double d   = Point::distance(p, value_.point());
    result.push(this, a.convert(this), d);

    if(Point::distance(p, value_.point(), axis_) <= result.largest())
    {

        // Visit other subtree...

        if(p.x(axis_) < value_.point().x(axis_))
        {
            if(right_) right(a)->kNearestNeighbours(a, p,k, result, depth+1);

        }
        else {

            if(left_) left(a)->kNearestNeighbours(a, p, k, result, depth+1);
        }
    }
}


template<class Traits>
typename SPNode<Traits>::NodeList SPNode<Traits>::kNearestNeighbours(Alloc& a,const Point& p, size_t k)
{
    NodeQueue queue(k);
    NodeList result;
    kNearestNeighbours(a,p,k,queue,0);
    queue.fill(result);
    return result;
}

template<class Traits>
void SPNode<Traits>::kNearestNeighboursBruteForce(Alloc& a,const Point& p, size_t k, NodeQueue& result, int depth)
{
    double d = Point::distance(p,value_.point());
    result.push(this, d);
    if(left_)  left(a)->kNearestNeighboursBruteForce(a, p, k, result, depth+1);
    if(right_) right(a)->kNearestNeighboursBruteForce(a, p, k, result, depth+1);
}

template<class Traits>
template<class Visitor>
void SPNode<Traits>::visit(Alloc& a,Visitor& v,int depth)
{
    v.enter(value_.point(), !left_ && !right_, depth);
    if(left_)  left(a)->visit(a, v, depth+1);
    if(right_) right(a)->visit(a, v, depth+1);
    v.leave(value_.point(),!left_ && !right_, depth);
}


template<class Traits>
void SPNode<Traits>::linkNodes(Alloc& a, SPNode<Traits>*& prev)
{
    if(prev) {
        prev->next(a, this);
    }
    prev = this;
    if(left_)  left(a)->linkNodes(a,  prev);
    if(right_) right(a)->linkNodes(a, prev);
}

template<class Traits>
typename SPNode<Traits>::NodeList SPNode<Traits>::kNearestNeighboursBruteForce(Alloc& a,const Point& p, size_t k)
{
    NodeQueue queue(k);
    NodeList result;
    kNearestNeighboursBruteForce(a,p,k,queue,0);
    queue.fill(result);
    return result;
}
//===

template<class Value>
struct sorter {
    int axis_;
    bool operator() (const Value& a,const Value& b)
        { return (a.point().x(axis_) < b.point().x(axis_)); }
    sorter(size_t axis) : axis_(axis) {}
};


template<class Traits>
template<typename ITER>
SPNode<Traits>* SPNode<Traits>::build(Alloc& a,
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

    std::nth_element(begin, begin + median, end, sorter<Value>(axis));

    ITER e2 = begin + median;
    ITER b2 = begin + median+1;


    SPNode* n = a.newNode2(*e2, axis,(SPNode*)0);

    n->left(a,build(a, begin, e2, depth + 1));
    n->right(a,build(a, b2,   end, depth + 1));

    return n;

}

template<class Traits>
void SPNode<Traits>::findInSphere(Alloc& a,const Point& p ,double radius, NodeList& result, int depth)
{
    if(p.x(axis_) < value_.point().x(axis_))
    {
        if(left_) left(a)->findInSphere(a, p, radius, result, depth+1);
    }
    else
    {
        if(right_) right(a)->findInSphere(a, p, radius, result, depth+1);
    }

    double d   = Point::distance(p, value_.point());
    if(d <= radius) {
        result.push_back(NodeInfo(this,d));
    }


    if(Point::distance(p, value_.point(), axis_) <= radius)
    {

        // Visit other subtree...

        if(p.x(axis_) < value_.point().x(axis_))
        {
            if(right_) right(a)->findInSphere(a, p,radius, result, depth+1);

        }
        else {

            if(left_) left(a)->findInSphere(a, p, radius, result, depth+1);
        }
    }
}


template<class Traits>
typename SPNode<Traits>::NodeList SPNode<Traits>::findInSphere(Alloc& a,const Point& p, double radius)
{
    NodeList result;
    findInSphere(a,p,radius,result,0);
    std::sort(result.begin(), result.end());
    return result;
}

template<class Traits>
void SPNode<Traits>::findInSphereBruteForce(Alloc& a,const Point& p, double radius, NodeList& result, int depth)
{
    double d = Point::distance(p,value_.point());
    if(d <= radius) {
        result.push_back(NodeInfo(this,d));
    }
    if(left_)  left(a)->findInSphereBruteForce(a, p, radius, result, depth+1);
    if(right_) right(a)->findInSphereBruteForce(a, p, radius, result, depth+1);
}

template<class Traits>
typename SPNode<Traits>::NodeList SPNode<Traits>::findInSphereBruteForce(Alloc& a,const Point& p, double radius)
{
    NodeList result;
    findInSphereBruteForce(a,p,radius,result,0);
    std::sort(result.begin(), result.end());
    return result;
}


} //namespace

#endif
