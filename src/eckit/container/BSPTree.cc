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
#include <eckit/eckit.h>

#include <stdio.h>
#include <limits>
#include <random>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "eckit/eckit.h"

namespace eckit {

template<class Point, class Alloc>
template<typename Container>
BSPNode<Point,Alloc>::BSPNode(const Container& p):
    point_(Point::mean(p)),
    left_(0),
    right_(0)
{
}

template<class Point, class Alloc>
BSPNodeInfo<Point,Alloc> BSPNode<Point,Alloc>::nearestNeighbour(Alloc& a,const Point& p)
{
    double max = std::numeric_limits<double>::max();
    BSPNode* best = 0;
    nearestNeighbour(a, p, best, max, 0);
    return NodeInfo(best, max);
}

template<class Point, class Alloc>
void BSPNode<Point,Alloc>::nearestNeighbour(Alloc& a,const Point& p, BSPNode*& best, double& max, int depth)
{
    if(left_ && right_) {
        const Point& l =   left(a)->point();
        const Point& r =   right(a)->point();

        double dl = Point::distance(p, l);
        double dr = Point::distance(p, r);

        if(dl <= dr) {
            left(a)->nearestNeighbour(a, p, best, max, depth+1);
        }
        else {
            right(a)->nearestNeighbour(a, p, best, max, depth+1);
        }
    }
    else
    {

        ASSERT(!left_ || !right_);

    double d   = Point::distance(p, point_);
    if(d < max) {
        max = d;
        best = this;
    }
    }

}


template<class Point, class Alloc>
BSPNodeInfo<Point,Alloc> BSPNode<Point,Alloc>::nearestNeighbourBruteForce(Alloc& a,const Point& p)
{
    double max = std::numeric_limits<double>::max();
    BSPNode* best = 0;
    nearestNeighbourBruteForce(a, p, best, max, 0);
    return NodeInfo(best, max);
}


template<class Point, class Alloc>
void BSPNode<Point,Alloc>::nearestNeighbourBruteForce(Alloc& a,const Point& p, BSPNode<Point,Alloc>*& best, double& max, int depth)
{
    if(left_ && right_) {
        right(a)->nearestNeighbourBruteForce(a, p, best, max, depth+1);
        left(a)->nearestNeighbourBruteForce(a, p, best, max, depth+1);
    }
    else
    {
        ASSERT(!left_ && !right_);

        double d   = Point::distance(p, point_);
        if(d < max) {
            max = d;
            best = this;
        }
    }
}

//===

template<class Point, class Alloc>
void BSPNode<Point,Alloc>::kNearestNeighbours(Alloc& a,const Point& p ,size_t k, NodeQueue& result, int depth)
{

}


template<class Point, class Alloc>
typename BSPNode<Point,Alloc>::NodeList BSPNode<Point,Alloc>::kNearestNeighbours(Alloc& a,const Point& p, size_t k)
{
    NodeQueue queue(k);
    NodeList result;
    kNearestNeighbours(a,p,k,queue,0);
    queue.fill(result);
    return result;
}

template<class Point, class Alloc>
void BSPNode<Point,Alloc>::kNearestNeighboursBruteForce(Alloc& a,const Point& p, size_t k, NodeQueue& result, int depth)
{
    double d = Point::distance(p,point_);
    result.push(this, d);
    if(left_)  left(a)->kNearestNeighboursBruteForce(a, p, k, result, depth+1);
    if(right_) right(a)->kNearestNeighboursBruteForce(a, p, k, result, depth+1);
}

template<class Point, class Alloc>
template<class Visitor>
void BSPNode<Point,Alloc>::visit(Alloc& a,Visitor& v,int depth)
{
    v.enter(point_, !left_ && !right_, depth);
    if(left_)  left(a)->visit(a, v, depth+1);
    if(right_) right(a)->visit(a, v, depth+1);
    v.leave(point_,!left_ && !right_, depth);
}


template<class Point, class Alloc>
typename BSPNode<Point,Alloc>::NodeList BSPNode<Point,Alloc>::kNearestNeighboursBruteForce(Alloc& a,const Point& p, size_t k)
{
    NodeQueue queue(k);
    NodeList result;
    kNearestNeighboursBruteForce(a, p, k, queue, 0);
    queue.fill(result);
    return result;
}
//===


template<class Point, class Alloc>
template<typename Container>
void BSPNode<Point,Alloc>::kmean(const Container& in, Container& ml, Container& mr, int depth)
{

    // Bisecting k-mean

    Point w = Point::mean(in);

    std::default_random_engine generator;
    std::uniform_int_distribution<size_t> distribution(0, in.size()-1);
    Point cl = in[distribution(generator)];
    Point cr = Point::symetrical(w, cl); // cr = w - (cl - w)

    ///ml.reserve(in.size());
    ///mr.reserve(in.size());

    std::vector<size_t> prev;
    std::vector<size_t> curr;

    for(;;) {
        ml.clear();
        mr.clear();
        curr.clear();

        size_t i = 0;

        for(typename Container::const_iterator j = in.begin(); j != in.end(); ++j, ++i) {
            double dl = Point::distance(cl, *j);
            double dr = Point::distance(cr, *j);
            if(dl <= dr) {
                ml.push_back(*j);
                curr.push_back(i);
            }
            else
            {
                mr.push_back(*j);
            }
        }

        ASSERT(ml.size());
        ASSERT(mr.size());

        Point wl = Point::mean(ml);
        Point wr = Point::mean(mr);

        //cout << Point::distance(cl, wl) << " - " << Point::distance(cr, wr) << std::endl;

        //if(cl == wl && cr == wr)
        if(curr == prev)
        {
            for(int i = 0; i < depth; i++)
                std::cout << "  ";
            std::cout << Point::distance(cl, wl) << " - " << Point::distance(cr, wr) << std::endl;
            //cout << "======================" << std::endl;
            break;
        }

        cl = wl;
        cr = wr;
        prev = curr;
    }


}

template<class Point, class Alloc>
template<typename Container>
BSPNode<Point,Alloc>* BSPNode<Point,Alloc>::build(Alloc& a, const Container& nodes, int depth)
{
    if(nodes.size() == 0)
        return 0;


    if(nodes.size() == 1)
        return a.newNode(nodes,(BSPNode*)0);

    //if(depth == 3)
    //    return a.newNode(nodes,(BSPNode*)0);

    Container  left;
    Container  right;

    kmean(nodes, left, right, depth);


    BSPNode* n = a.newNode(nodes, (BSPNode*)0);

    n->left(a,build(a, left, depth + 1));
    n->right(a,build(a, right, depth + 1));


    return n;

}

template<class Point, class Alloc>
void BSPNode<Point,Alloc>::findInSphere(Alloc& a,const Point& p ,double radius, NodeList& result, int depth)
{
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


template<class Point, class Alloc>
typename BSPNode<Point,Alloc>::NodeList BSPNode<Point,Alloc>::findInSphere(Alloc& a,const Point& p, double radius)
{
    NodeList result;
    findInSphere(a,p,radius,result,0);
    std::sort(result.begin(), result.end());
    return result;
}

template<class Point, class Alloc>
void BSPNode<Point,Alloc>::findInSphereBruteForce(Alloc& a,const Point& p, double radius, NodeList& result, int depth)
{
    /*
    double d = Point::distance(p,point_);
    if(d <= radius) {
        result.push_back(NodeInfo(this,d));
    }
    if(left_)  left(a)->findInSphereBruteForce(a, p, radius, result, depth+1);
    if(right_) right(a)->findInSphereBruteForce(a, p, radius, result, depth+1);
    */
}

template<class Point, class Alloc>
typename BSPNode<Point,Alloc>::NodeList BSPNode<Point,Alloc>::findInSphereBruteForce(Alloc& a,const Point& p, double radius)
{
    NodeList result;
    findInSphereBruteForce(a,p,radius,result,0);
    std::sort(result.begin(), result.end());
    return result;
}

} //namespace

#endif
