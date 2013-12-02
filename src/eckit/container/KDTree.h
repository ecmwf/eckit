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


#include "KDPoint.h"
#include "KDMemory.h"

namespace eckit {


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

    friend std::ostream& operator<<(std::ostream& s,const KDNodeInfo& p)
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

    template<typename ITER>
    static KDNode* build(Alloc& a,const ITER& begin, const ITER& end, int depth = 0);

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

    /// ITER must be a random access iterator
    template<typename ITER>
    void build(const ITER& begin, const ITER& end)
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
