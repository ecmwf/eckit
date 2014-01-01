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
#include "eckit/container/KDMemory.h"

//#include <cmath>
#include <limits>
#include <cmath>



// Implements a tree

namespace eckit {


template<class Traits>
class BSPNode;

template<class Traits>
struct BSPNodeInfo {
    typedef typename Traits::Point   Point;
    typedef typename Traits::Payload Payload;
    typedef typename Traits::Alloc   Alloc;
    typedef typename Traits::Partition   Partition;

    typedef BSPNode<Traits>     Node;

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

    friend std::ostream& operator<<(std::ostream& s,const NodeList& p)
    {
        s << "[";
        std::string sep = "";
        for(typename NodeList::const_iterator j = p.begin() ; j != p.end(); ++j) {
            s << sep << *j;
            sep = ",";
        }
        s << "]";
        return s;
    }
};

template<class Traits>
class BSPNodeQueue {
public:
    typedef typename Traits::Point   Point;
    typedef typename Traits::Payload Payload;
    typedef typename Traits::Alloc   Alloc;
    typedef typename Traits::Partition   Partition;


    typedef BSPNode<Traits>              Node;
    typedef BSPNodeInfo<Traits>          NodeInfo;
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

    bool incomplete() const {
        return queue_.size() < k_;
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

template<class Traits>
class BSPValue {
public:
    typedef typename Traits::Point   Point;
    typedef typename Traits::Payload Payload;

    Point point_;
    Payload payload_;

public:

    BSPValue(const Point& point, const Payload& payload):
        point_(point), payload_(payload) {}

    const Point& point() const   { return point_; }
    const Payload& payload() const { return payload_; }


    Point& point()    { return point_; } // FIXME: remove this one
    Payload& payload()  { return payload_; }


    void point(const Point& p) const   {  point_ = p; }
    void payload(const Payload& p) const {  payload_ = p; }

    void print(std::ostream& o) const {
        o << "(point=" << point_ << ",payload=" << payload_ << ")";
    }

    friend std::ostream& operator<<(std::ostream& o, const BSPValue& t) {
        t.print(o);
        return o;
    }
};

template<class Point>
class BSPHyperPlane {
    Point normal_;
    double d_;
public:

    BSPHyperPlane():
        normal_(), d_() {}

    BSPHyperPlane(const Point& normal, const Point& point):
        normal_(Point::normalize(normal)), d_(-Point::dot(normal_, point)) {}

    double position(const Point& p) const {
        return Point::dot(p, normal_) + d_;
    }

    const Point& normal() const { return normal_; }
    double d() const { return d_; }
};

template<class Traits>
class BSPNode {
public:
    typedef typename Traits::Point   Point;
    typedef typename Traits::Payload Payload;
    typedef typename Traits::Alloc   Alloc;
    typedef typename Traits::Partition   Partition;
    typedef BSPHyperPlane<Point> HyperPlane;
    typedef  BSPValue<Traits>     Value;

private:

    Value value_;

    HyperPlane plane_;
    double dist_; // Distance to parent's hyperplane


    typedef typename Alloc::Ptr Ptr;
    Ptr left_;
    Ptr right_;

    friend struct KDMemory;

public:
    typedef BSPNodeQueue<Traits>      NodeQueue;
    typedef BSPNodeInfo<Traits>       NodeInfo;
    typedef typename NodeQueue::NodeList  NodeList;


public:

    BSPNode(const Value&, const HyperPlane&, double dist);

    NodeInfo nearestNeighbour(Alloc& a,const Point& p);
    NodeList findInSphere(Alloc& a,const Point& p, double radius);
    NodeList kNearestNeighbours(Alloc& a,const Point& p,size_t k);

    const Point& point() const { return value_.point(); }

    template<typename Container>
    static BSPNode* build(Alloc& a, Partition& p, const Container& nodes, double, int depth= 0);

    template<typename Container>
    static double distanceToPlane(const Container& in, const HyperPlane& p) ;

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




template<class Traits = KDMemory>
class BSPTree {

public:
    typedef typename Traits::Point   Point;
    typedef typename Traits::Payload Payload;
    typedef typename Traits::Alloc   Alloc;
    typedef typename Traits::Partition   Partition;


    typedef typename Alloc::Ptr Ptr;
    typedef BSPNode<Traits> Node;
    typedef          Point PointType;
    typedef typename BSPNode<Traits>::NodeList NodeList;
    typedef          BSPNodeInfo<Traits>       NodeInfo;
    typedef typename Node::Value Value;

    Alloc alloc_;
    Ptr   root_;
    Partition partition_;

public:

    BSPTree(const Alloc& alloc = Alloc()): alloc_(alloc), root_(alloc.root())  {}

    ~BSPTree() {
        alloc_.deleteNode(root_,(Node*)0);
    }

    void verbose() {

    }


    template<typename Container>
    void build(const Container& nodes)
    {
        root_ = alloc_.convert(Node::build(alloc_, partition_, nodes, 0.0));
    }

    NodeInfo nearestNeighbour(const Point& p)
    {
        alloc_.statsCall();
        return alloc_.convert(root_,(Node*)0)->nearestNeighbour(alloc_, p);
    }

    NodeList findInSphere(const Point& p,double radius)
    {
        alloc_.statsCall();
        return alloc_.convert(root_,(Node*)0)->findInSphere(alloc_, p, radius);
    }

    NodeList kNearestNeighbours(const Point& p, size_t k)
    {
        alloc_.statsCall();
        return alloc_.convert(root_,(Node*)0)->kNearestNeighbours(alloc_, p, k);
    }

    // For testing only...
    NodeInfo nearestNeighbourBruteForce(const Point& p)
    {
        alloc_.statsCall();
        return alloc_.convert(root_,(Node*)0)->nearestNeighbourBruteForce(alloc_, p);
    }


    NodeList findInSphereBruteForce(const Point& p,double radius)
    {
        alloc_.statsCall();
        return alloc_.convert(root_,(Node*)0)->findInSphereBruteForce(alloc_, p, radius);
    }

    NodeList kNearestNeighboursBruteForce(const Point& p,size_t k)
    {
        alloc_.statsCall();
        return alloc_.convert(root_,(Node*)0)->kNearestNeighboursBruteForce(alloc_, p, k);
    }


    template<class Visitor>
    void visit(Visitor& v)
    {
        return alloc_.convert(root_,(Node*)0)->visit(alloc_, v);
    }

    void statsReset() {
        alloc_.statsReset();
    }

    void statsPrint(std::ostream& o, bool fancy) const {
        if(fancy) o << *this << ": ";
        alloc_.statsPrint(o, fancy);
    }

    void print(std::ostream& o) const {
        o << "BSPTree<" << Partition::name() << ">";
    }

    friend std::ostream& operator<<(std::ostream& o, const BSPTree& t) {
        t.print(o);
        return o;
    }
};




} // Name space

#include "BSPTree.cc"

#endif
