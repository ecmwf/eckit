/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef KDTree_H
#define KDTree_H

#include <eckit/eckit.h>

//#include <cmath>
#include <limits>
#include <cmath>


namespace eckit {


template<class T, int SIZE = 2>
class KDPoint {
protected:
    double x_[SIZE];

public:

    typedef T Payload;

    Payload payload_;

    double x(int axis) const { return x_[axis]; }

    KDPoint(): payload_()
    {
        std::fill(x_, x_+size(), 0);
    }

    KDPoint(double x, double y, const Payload& payload): payload_(payload)
    {
        x_[0] = x;
        x_[1] = y;
    }
    
    bool operator<(const KDPoint& other) const
    { return lexicographical_compare(x_,x_ + SIZE, other.x_, other.x_ + SIZE); }

    static size_t size() { return SIZE; }

    friend ostream& operator<<(ostream& s,const KDPoint& p)
    {
        s << '(' << p.x_[0] << "," << p.x_[1] << ' ' << p.payload_ << ')';
        //s << '(' << p.x_[0] << "," << p.x_[1] << ')';
        return s;
    }

    static  double distance(const KDPoint& p1, const KDPoint& p2)
    {
        double m = 0;
        for(size_t i = 0; i < size(); i++)
        {
            double dx =  p1.x_[i]  - p2.x_[i];
            m += dx*dx;
        }
        return std::sqrt(m);
    }

    // Distance along one axis
    static  double distance(const KDPoint& p1, const KDPoint& p2, int axis)
    {
        return fabs(p1.x_[axis] - p2.x_[axis]);
    }

    // For projecting a point on a line
    static double dot(const KDPoint& p1, const KDPoint& p2)
    {
        double m = 0;
        for(size_t i = 0; i < size(); i++)
        {
            m += p1.x_[i] * p2.x_[i];
        }
        return m;
    }

    static KDPoint add(const KDPoint& p1, const KDPoint& p2)
    {
        KDPoint q(p1);
        for(size_t i = 0; i < size(); i++)
        {
            q.x_[i] += p2.x_[i];
        }
        return q;
    }

    static KDPoint sub(const KDPoint& p1, const KDPoint& p2)
    {
        KDPoint q(p1);
        for(size_t i = 0; i < size(); i++)
        {
            q.x_[i] -= p2.x_[i];
        }
        return q;
    }

    static KDPoint mul(const KDPoint& p, double m)
    {
        KDPoint q(p);
        for(size_t i = 0; i < size(); i++)
        {
            q.x_[i] *= m;
        }
        return q;
    }

    static KDPoint div(const KDPoint& p, double m)
    {
        KDPoint q(p);
        for(size_t i = 0; i < size(); i++)
        {
            q.x_[i] /= m;
        }
        return q;
    }

    static KDPoint normalize(const KDPoint& p)
    {
        KDPoint zero;
        return mul(p,distance(p, zero));
    }

    /*

    u = diff(a,b);
    v = diff(a,p);

    U = normalize(u);
    x = mul(U,dot(U,v));

    // P = project(p) on line(a,b)
    P = add(a,x)

    // Distance to line
    ditance(p,P)



    */

};

struct KDMemory {
    typedef void* Ptr;

    template<class Node>
    Ptr convert(Node* p) { return p; }

    template<class Node>
    Node* convert(Ptr p,  const Node*) { return static_cast<Node*>(p); }

    template<class Node,class Point>
    Node* newNode(const Point& p, size_t a, const Node*) { return new Node(p,a); }

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
class KDNode;

template<class Point, class Alloc>
struct KDNodeInfo {
    typedef KDNode<Point,Alloc>     Node;

    const Node* node_;
    double distance_;

public:
    explicit KDNodeInfo(const Node* node = 0, double distance = 0):
        node_(node), distance_(distance) {}

    bool operator<(const KDNodeInfo& other) const
    { return distance_ < other.distance_; }

    typedef vector<KDNodeInfo> NodeList;

    const Point& point() const { return node_->point(); }
    double distance() const    { return distance_; }

    friend ostream& operator<<(ostream& s,const KDNodeInfo& p)
    {
        s << "[point=" << p.point() << ",distance=" << p.distance() << "]";
        return s;
    }
};

template<class Point, class Alloc>
class KDNodeQueue {
public:
    typedef KDNode<Point,Alloc>              Node;
    typedef KDNodeInfo<Point,Alloc>          NodeInfo;
    typedef typename NodeInfo::NodeList      NodeList;

private:
    size_t k_;
    std::priority_queue<NodeInfo> queue_;

public:


    KDNodeQueue(size_t k) : k_(k) {}

    void push(Node* n, double d)
    {
        queue_.push(NodeInfo(n,d));
        while(queue_.size() > k_) { queue_.pop();}
    }

    double largest() const {
        return queue_.size() ? queue_.top().distance_ : numeric_limits<double>::max();
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
class KDNode {
private:

    Point  point_;
    int    axis_;

    typedef typename Alloc::Ptr Ptr;
    Ptr left_;
    Ptr right_;

    friend struct KDMemory;

public:
    typedef KDNodeQueue<Point,Alloc>      NodeQueue;
    typedef KDNodeInfo<Point,Alloc>       NodeInfo;
    typedef typename NodeQueue::NodeList  NodeList;

public:
    KDNode(const Point& p,int axis);

    NodeInfo nearestNeighbour(Alloc& a,const Point& p);
    NodeList findInSphere(Alloc& a,const Point& p, double radius);
    NodeList kNearestNeighbours(Alloc& a,const Point& p,size_t k);

    const Point& point() const { return point_; }

    static KDNode* build(Alloc& a,const typename vector<Point>::iterator& begin, const typename vector<Point>::iterator& end, int depth = 0);

    // For testing only

    NodeInfo nearestNeighbourBruteForce(Alloc& a,const Point& p);
    NodeList findInSphereBruteForce(Alloc& a,const Point& p, double radius);
    NodeList kNearestNeighboursBruteForce(Alloc& a,const Point& p,size_t k);

    // -------
    template<class Visitor>
    void visit(Alloc& a,Visitor& v, int depth = 0);


private:
    void nearestNeighbour(Alloc& a,const Point& p, KDNode*& best, double& max, int depth);
    void nearestNeighbourBruteForce(Alloc& a,const Point& p,KDNode*& best,double& max, int depth);
    void findInSphere(Alloc& a,const Point& p ,double radius, NodeList& result, int depth) ;
    void findInSphereBruteForce(Alloc& a,const Point& p, double radius, NodeList& result, int depth) ;
    void kNearestNeighbours(Alloc& a,const Point& p ,size_t k, NodeQueue& result, int depth) ;
    void kNearestNeighboursBruteForce(Alloc& a,const Point& p, size_t k, NodeQueue& result, int depth) ;

    //==========================

    KDNode* left(Alloc& a)  const { return a.convert(left_, this);   }
    KDNode* right(Alloc& a) const { return a.convert(right_, this);  }

    void  left(Alloc& a,KDNode* n)  { left_  = a.convert(n); }
    void  right(Alloc& a,KDNode* n) { right_ = a.convert(n); }

};




template<class Point, class Alloc = KDMemory>
class KDTree {

public:


    typedef typename Alloc::Ptr Ptr;
    typedef KDNode<Point,Alloc> Node;

    typedef typename KDNode<Point,Alloc>::NodeList NodeList;
    typedef          KDNodeInfo<Point,Alloc>       NodeInfo;

    Alloc alloc_;
    Ptr   root_;

public:

    KDTree(const Alloc& alloc = Alloc()): alloc_(alloc), root_(0) {}
    ~KDTree() {
        alloc_.deleteNode(root_,(Node*)0);
    }

    void build(const typename vector<Point>::iterator& begin, const typename vector<Point>::iterator& end)
    {
        root_ = alloc_.convert(Node::build(alloc_,begin,end));
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

#include "KDTree.cc"

#endif
