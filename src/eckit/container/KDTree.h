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
class KDTreeIterator;

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

    typedef std::vector<KDNodeInfo> NodeList;

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
struct KDMetadata {
    Point  offset_;
    Point  scale_;
};

template<class Point, class Alloc>
class KDNode {
private:

    Point  point_;
    size_t    axis_;

    typedef typename Alloc::Ptr Ptr;
    Ptr left_;
    Ptr right_;

    friend struct KDMemory;

public:
    typedef KDNodeQueue<Point,Alloc>      NodeQueue;
    typedef KDNodeInfo<Point,Alloc>       NodeInfo;
    typedef typename NodeQueue::NodeList  NodeList;

public:
    KDNode(const std::pair<const Point&,size_t>&);

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

    friend class KDTreeIterator<Point,Alloc>;

};

template<class Point, class Alloc = KDMemory>
class KDTreeIterator {
    typedef typename Alloc::Ptr Ptr;
    typedef KDNode<Point,Alloc> Node;
    Alloc& alloc_;
    Ptr   ptr_;

    void advance();

public:
    KDTreeIterator(Alloc& alloc, Ptr ptr):
        alloc_(alloc), ptr_(ptr) {}

    bool operator !=(const KDTreeIterator& other)
        { return ptr_ != other.ptr_; }

    operator const Point*() {
        return &(alloc_.convert(ptr_,(Node*)0)->point());
    }

    const Point* operator->() {
        return &(alloc_.convert(ptr_,(Node*)0)->point());
    }


    KDTreeIterator& operator++() {
        advance();
        return *this;
    }
};


template<class Point, class Alloc = KDMemory>
class KDTree {

public:

    typedef typename Alloc::Ptr Ptr;
    typedef KDNode<Point,Alloc> Node;
    typedef KDMetadata<Point,Alloc> Metadata;

    typedef          Point PointType;
    typedef typename KDNode<Point,Alloc>::NodeList NodeList;
    typedef          KDNodeInfo<Point,Alloc>       NodeInfo;

    Alloc alloc_;
    Ptr   root_;
    Metadata meta_;

    typedef KDTreeIterator<Point,Alloc> iterator;

public:

    KDTree(const Alloc& alloc = Alloc()): alloc_(alloc), root_(0) {}
    ~KDTree() {
        alloc_.deleteNode(root_,(Node*)0);
    }

    /// ITER must be a random access iterator
    /// WARNING: container is changed (sorted)
    template<typename ITER>
    void build(ITER begin, ITER end)
    {
        root_ = alloc_.convert(Node::build(alloc_,begin,end));
    }

    void setMetadata(const Point& offset, const Point& scale) {
        meta_.offset_ = offset;
        meta_.scale_  = scale;
        alloc_.setMetadata(meta_);
    }

    void getMetadata(Point& offset, Point& scale) {
        alloc_.getMetadata(meta_);
        offset = meta_.offset_;
        scale  = meta_.scale_;
    }

    /// Container must be a random access
    /// WARNING: container is changed (sorted)
    template<typename Container>
    void build(Container& c)
    {
        typename Container::iterator b = c.begin();
        typename Container::iterator e = c.end();
        build(b, e);
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

    void statsReset() {
        alloc_.statsReset();
    }

    void statsPrint(std::ostream& o, bool fancy) const {
        if(fancy) o << *this << ": ";
        alloc_.statsPrint(o, fancy);
    }

    void print(std::ostream& o) const {
        o << "KDTree";
    }

    friend std::ostream& operator<<(std::ostream& o, const KDTree& t) {
        t.print(o);
        return o;
    }

    iterator begin() {
        return iterator(alloc_, root_);
    }

    iterator end() {
        return iterator(alloc_, 0);
    }
};

} // Name space

#include "KDTree.cc"

#endif
