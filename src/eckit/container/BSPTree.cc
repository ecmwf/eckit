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
#include <eckit/eckit.h>

#include <stdio.h>
#include <limits>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


namespace eckit {



template<class Point, class Alloc>
BSPNode<Point,Alloc>::BSPNode(const vector<Point>& p):
    // point_(p),
    left_(0),
    right_(0)
{
}

template<class Point, class Alloc>
BSPNodeInfo<Point,Alloc> BSPNode<Point,Alloc>::nearestNeighbour(Alloc& a,const Point& p)
{
    double max = numeric_limits<double>::max();
    BSPNode* best = 0;
    nearestNeighbour(a, p, best, max, 0);
    return NodeInfo(best,max);
}

template<class Point, class Alloc>
void BSPNode<Point,Alloc>::nearestNeighbour(Alloc& a,const Point& p, BSPNode*& best, double& max, int depth)
{
    /*
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
*/
}


template<class Point, class Alloc>
BSPNodeInfo<Point,Alloc> BSPNode<Point,Alloc>::nearestNeighbourBruteForce(Alloc& a,const Point& p)
{
    double max = numeric_limits<double>::max();
    BSPNode* best = 0;
    nearestNeighbourBruteForce(a, p, best, max, 0);
    return NodeInfo(best,max);
}


template<class Point, class Alloc>
void BSPNode<Point,Alloc>::nearestNeighbourBruteForce(Alloc& a,const Point& p, BSPNode<Point,Alloc>*& best, double& max, int depth)
{
    /*
    double d = Point::distance(p,point_);
    if(d < max)
    {
        best = this;
        max  = d;
    }

    if(left_)  left(a)->nearestNeighbourBruteForce(a, p, best, max, depth+1);
    if(right_) right(a)->nearestNeighbourBruteForce(a, p, best, max, depth+1);
    */
}

//===

template<class Point, class Alloc>
void BSPNode<Point,Alloc>::kNearestNeighbours(Alloc& a,const Point& p ,size_t k, NodeQueue& result, int depth)
{
    /*
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
    */
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
    /*
    double d = Point::distance(p,point_);
    result.push(this, d);
    if(left_)  left(a)->kNearestNeighboursBruteForce(a, p, k, result, depth+1);
    if(right_) right(a)->kNearestNeighboursBruteForce(a, p, k, result, depth+1);
    */
}

template<class Point, class Alloc>
template<class Visitor>
void BSPNode<Point,Alloc>::visit(Alloc& a,Visitor& v,int depth)
{
    /*
    v.enter(point_, !left_ && !right_, depth);
    if(left_)  left(a)->visit(a, v, depth+1);
    if(right_) right(a)->visit(a, v, depth+1);
    v.leave(point_,!left_ && !right_, depth);
    */
}


template<class Point, class Alloc>
typename BSPNode<Point,Alloc>::NodeList BSPNode<Point,Alloc>::kNearestNeighboursBruteForce(Alloc& a,const Point& p, size_t k)
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
void BSPNode<Point,Alloc>::kmean(const vector<Point>& in, vector<Point>& ml, vector<Point>& mr)
{

    // Bisecting k-mean

    Point w = Point::mean(in);

    std::default_random_engine generator;
    std::uniform_int_distribution<size_t> distribution(0, in.size()-1);
    Point cl = in[distribution(generator)];
    Point cr = Point::symetrical(w, cl); // cr = w - (cl - w)

    ml.reserve(in.size());
    mr.reserve(in.size());

    for(;;) {
        ml.clear();
        mr.clear();

        for(typename vector<Point>::const_iterator j = in.begin(); j != in.end(); ++j) {
            double dl = Point::distance(cl, *j);
            double dr = Point::distance(cr, *j);
            if(dl <= dr) {
                ml.push_back(*j);
            }
            else
            {
                mr.push_back(*j);
            }
        }


        Point wl = Point::mean(ml);
        Point wr = Point::mean(mr);


        if(cl == wl && cr == wr) {
            cout << Point::distance(cl, wl) << " - " << Point::distance(cr, wr) << endl;
            break;
        }

        cl = wl;
        cr = wr;
    }


}

template<class Point, class Alloc>
BSPNode<Point,Alloc>* BSPNode<Point,Alloc>::build(Alloc& a, const vector<Point>& nodes, int depth)
{
    if(nodes.size() == 0)
        return 0;


    if(nodes.size() == 1)
        return a.newNode(nodes,(BSPNode*)0);

    //if(depth == 3)
      //  return a.newNode(nodes,(BSPNode*)0);

    vector<Point>  left;
    vector<Point>  right;

    kmean(nodes, left, right);


    BSPNode* n = a.newNode(nodes,(BSPNode*)0);

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
