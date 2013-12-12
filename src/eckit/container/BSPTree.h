/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef BSPTree_H
#define BSPTree_H

#include "eckit/eckit.h"

//#include <cmath>
#include <limits>
#include <cmath>

// Implements a tree

namespace eckit {


struct BSPMemory {
    typedef void* Ptr;

    template<class Node>
    Ptr convert(Node* p) { return p; }

    template<class Node>
    Node* convert(Ptr p,  const Node*) { return static_cast<Node*>(p); }

    template<class Node,class Point>
    Node* newNode(const std::vector<Point>& p, const Node*) { return new Node(p); }

    template<class Node>
    void deleteNode(Ptr p, const Node*) {
        Node* n = static_cast<Node*>(p);
        if(n) {
            deleteNode(n->left_,n);
            deleteNode(n->right_,n);
            delete n;
        }
    }
};

template<class Point, class Alloc>
class BSPNode;

template<class Point, class Alloc>
struct BSPNodeInfo {
    typedef BSPNode<Point,Alloc>     Node;

    const Node* node_;
    double distance_;

public:
    explicit BSPNodeInfo(const Node* node = 0, double distance = 0):
        node_(node), distance_(distance) {}

    bool operator<(const BSPNodeInfo& other) const
    { return distance_ < other.distance_; }

    typedef std::vector<BSPNodeInfo> NodeList;

    const Point& point() const { return node_->point(); }
    double distance() const    { return distance_; }

    friend std::ostream& operator<<(std::ostream& s,const BSPNodeInfo& p)
    {
        s << "[point=" << p.point() << ",distance=" << p.distance() << "]";
        return s;
    }
};

template<class Point, class Alloc>
class BSPNodeQueue {
public:
    typedef BSPNode<Point,Alloc>              Node;
    typedef BSPNodeInfo<Point,Alloc>          NodeInfo;
    typedef typename NodeInfo::NodeList      NodeList;

private:
    size_t k_;
    std::priority_queue<NodeInfo> queue_;

public:


    BSPNodeQueue(size_t k) : k_(k) {}

    void push(Node* n, double d)
    {
        queue_.push(NodeInfo(n,d));
        while(queue_.size() > k_) { queue_.pop();}
    }

    double largest() const {
        return queue_.size() ? queue_.top().distance_ : std::numeric_limits<double>::max();
    }

    void fill(NodeList& v) {
        v.reserve(k_);
        while(!queue_.empty()) {
            v.push_back(queue_.top());
            queue_.pop();
        }
        std::sort(v.begin(),v.end());
    }

};

template<class Point, class Alloc>
class BSPNode {
private:

    Point point_;

    // The hyperplane is define by the vector between 2 centres passing through the middle point
    double d_;
    double n_;
    Point vec_;

    typedef typename Alloc::Ptr Ptr;
    Ptr left_;
    Ptr right_;

    friend struct BSPMemory;

public:
    typedef BSPNodeQueue<Point,Alloc>      NodeQueue;
    typedef BSPNodeInfo<Point,Alloc>       NodeInfo;
    typedef typename NodeQueue::NodeList  NodeList;

public:

    template<typename Container>
    BSPNode(const Container& p);

    NodeInfo nearestNeighbour(Alloc& a,const Point& p);
    NodeList findInSphere(Alloc& a,const Point& p, double radius);
    NodeList kNearestNeighbours(Alloc& a,const Point& p,size_t k);

    const Point& point() const { return point_; }

    template<typename Container>
    static BSPNode* build(Alloc& a, const Container& nodes, int depth= 0);

    template<typename Container>
    static void kmean(const Container& in, Container& ml, Container& mr, Point& l, Point& r, int depth) ;

    // For testing only

    NodeInfo nearestNeighbourBruteForce(Alloc& a,const Point& p);
    NodeList findInSphereBruteForce(Alloc& a,const Point& p, double radius);
    NodeList kNearestNeighboursBruteForce(Alloc& a,const Point& p,size_t k);

    // -------
    template<class Visitor>
    void visit(Alloc& a,Visitor& v, int depth = 0);


private:
    void nearestNeighbour(Alloc& a,const Point& p, BSPNode*& best, double& max, int depth);
    void nearestNeighbourBruteForce(Alloc& a,const Point& p,BSPNode*& best,double& max, int depth);
    void findInSphere(Alloc& a,const Point& p ,double radius, NodeList& result, int depth) ;
    void findInSphereBruteForce(Alloc& a,const Point& p, double radius, NodeList& result, int depth) ;
    void kNearestNeighbours(Alloc& a,const Point& p ,size_t k, NodeQueue& result, int depth) ;
    void kNearestNeighboursBruteForce(Alloc& a,const Point& p, size_t k, NodeQueue& result, int depth) ;

    //==========================

    BSPNode* left(Alloc& a)  const { return a.convert(left_, this);   }
    BSPNode* right(Alloc& a) const { return a.convert(right_, this);  }

    void  left(Alloc& a,BSPNode* n)  { left_  = a.convert(n); }
    void  right(Alloc& a,BSPNode* n) { right_ = a.convert(n); }

};




template<class Point, class Alloc = BSPMemory>
class BSPTree {

public:


    typedef typename Alloc::Ptr Ptr;
    typedef BSPNode<Point,Alloc> Node;

    typedef typename BSPNode<Point,Alloc>::NodeList NodeList;
    typedef          BSPNodeInfo<Point,Alloc>       NodeInfo;

    Alloc alloc_;
    Ptr   root_;

public:

    BSPTree(const Alloc& alloc = Alloc()): alloc_(alloc), root_(0) {}
    ~BSPTree() {
        alloc_.deleteNode(root_,(Node*)0);
    }

    template<typename Container>
    void build(const Container& nodes)
    {
        root_ = alloc_.convert(Node::build(alloc_, nodes));
    }

    NodeInfo nearestNeighbour(const Point& p)
    {
        return alloc_.convert(root_,(Node*)0)->nearestNeighbour(alloc_, p);
    }

    NodeList findInSphere(const Point& p,double radius)
    {
        return alloc_.convert(root_,(Node*)0)->findInSphere(alloc_, p, radius);
    }

    NodeList kNearestNeighbours(const Point& p, size_t k)
    {
        return alloc_.convert(root_,(Node*)0)->kNearestNeighbours(alloc_, p, k);
    }

    // For testing only...
    NodeInfo nearestNeighbourBruteForce(const Point& p)
    {
        return alloc_.convert(root_,(Node*)0)->nearestNeighbourBruteForce(alloc_, p);
    }


    NodeList findInSphereBruteForce(const Point& p,double radius)
    {
        return alloc_.convert(root_,(Node*)0)->findInSphereBruteForce(alloc_, p, radius);
    }

    NodeList kNearestNeighboursBruteForce(const Point& p,size_t k)
    {
        return alloc_.convert(root_,(Node*)0)->kNearestNeighboursBruteForce(alloc_, p, k);
    }


    template<class Visitor>
    void visit(Visitor& v)
    {
        return alloc_.convert(root_,(Node*)0)->visit(alloc_, v);
    }


};

} // Name space

#include "BSPTree.cc"

#endif
