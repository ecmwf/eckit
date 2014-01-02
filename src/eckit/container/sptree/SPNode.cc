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
SPNode<Traits>::SPNode(const Value& value):
    value_(value),
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
    nearestNeighbourX(a, p, best, max, 0);
    return NodeInfo(best,a.convert(best), max);
}




template<class Traits>
SPNodeInfo<Traits> SPNode<Traits>::nearestNeighbourBruteForce(Alloc& a,const Point& p)
{
    double max = std::numeric_limits<double>::max();
    SPNode* best = 0;
    nearestNeighbourBruteForceX(a, p, best, max, 0);
    return NodeInfo(best,0,max);
}


template<class Traits>
void SPNode<Traits>::nearestNeighbourBruteForceX(Alloc& a,const Point& p, SPNode<Traits>*& best, double& max, int depth)
{
    double d = Point::distance(p,value_.point());
    if(d < max)
    {
        best = this;
        max  = d;
    }

    if(left_)  left(a)->nearestNeighbourBruteForceX(a, p, best, max, depth+1);
    if(right_) right(a)->nearestNeighbourBruteForceX(a, p, best, max, depth+1);
}

//===



template<class Traits>
typename SPNode<Traits>::NodeList SPNode<Traits>::kNearestNeighbours(Alloc& a,const Point& p, size_t k)
{
    NodeQueue queue(k);
    NodeList result;
    kNearestNeighboursX(a,p,k,queue,0);
    queue.fill(result);
    return result;
}

template<class Traits>
void SPNode<Traits>::kNearestNeighboursBruteForceX(Alloc& a,const Point& p, size_t k, NodeQueue& result, int depth)
{
    double d = Point::distance(p,value_.point());
    result.push(this, 0, d);
    if(left_)  left(a)->kNearestNeighboursBruteForceX(a, p, k, result, depth+1);
    if(right_) right(a)->kNearestNeighboursBruteForceX(a, p, k, result, depth+1);
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
    kNearestNeighboursBruteForceX(a,p,k,queue,0);
    queue.fill(result);
    return result;
}
//===

#if 0

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

#endif


template<class Traits>
typename SPNode<Traits>::NodeList SPNode<Traits>::findInSphere(Alloc& a,const Point& p, double radius)
{
    NodeList result;
    findInSphereX(a,p,radius,result,0);
    std::sort(result.begin(), result.end());
    return result;
}

template<class Traits>
void SPNode<Traits>::findInSphereBruteForceX(Alloc& a,const Point& p, double radius, NodeList& result, int depth)
{
    double d = Point::distance(p,value_.point());
    if(d <= radius) {
        result.push_back(NodeInfo(this,0,d));
    }
    if(left_)  left(a)->findInSphereBruteForceX(a, p, radius, result, depth+1);
    if(right_) right(a)->findInSphereBruteForceX(a, p, radius, result, depth+1);
}

template<class Traits>
typename SPNode<Traits>::NodeList SPNode<Traits>::findInSphereBruteForce(Alloc& a,const Point& p, double radius)
{
    NodeList result;
    findInSphereBruteForceX(a,p,radius,result,0);
    std::sort(result.begin(), result.end());
    return result;
}


} //namespace

#endif
