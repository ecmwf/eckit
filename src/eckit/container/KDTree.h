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


template<class Traits>
class KDNode;

template<class Traits>
class KDTreeIterator;

template<class Traits>
struct KDNodeInfo {

    typedef typename Traits::Point   Point;
    typedef typename Traits::Payload Payload;
    typedef typename Traits::Alloc   Alloc;

    typedef KDNode<Traits> Node;

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

template<class Traits>
class KDNodeQueue {
public:

    typedef typename Traits::Point   Point;
    typedef typename Traits::Payload Payload;
    typedef typename Traits::Alloc   Alloc;

    typedef KDNode<Traits>              Node;
    typedef KDNodeInfo<Traits>          NodeInfo;
    typedef typename NodeInfo::NodeList NodeList;

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

template<class Traits>
struct KDMetadata {

    typedef typename Traits::Point   Point;

    Point  offset_;
    Point  scale_;
};

template<class Traits>
class KDValue {
public:
    typedef typename Traits::Point   Point;
    typedef typename Traits::Payload Payload;

    Point point_;
    Payload payload_;

public:

    KDValue(const Point& point, const Payload& payload):
        point_(point), payload_(payload) {}

    const Point& point() const   { return point_; }
    const Payload& payload() const { return payload_; }

     Point& point()    { return point_; }
     Payload& payload()  { return payload_; }


    void point(const Point& p) const   {  point_ = p; }
    void payload(const Payload& p) const {  payload_ = p; }
};

template<class Traits>
class KDNode {
public:
    typedef typename Traits::Point   Point;
    typedef typename Traits::Payload Payload;
    typedef typename Traits::Alloc   Alloc;

    typedef KDNodeInfo<Traits>          NodeInfo;
    typedef typename NodeInfo::NodeList NodeList;
    typedef KDValue<Traits>             Value;
    typedef KDNodeQueue<Traits>         NodeQueue;

private:

    Value     value_;
    size_t    axis_;

    typedef typename Alloc::Ptr Ptr;
    Ptr left_;
    Ptr right_;
    Ptr next_; // For fast transversal

    friend struct KDMemory;


public:
    KDNode(const Value& value, size_t axis);

    NodeInfo nearestNeighbour(Alloc& a,const Point& p);
    NodeList findInSphere(Alloc& a,const Point& p, double radius);
    NodeList kNearestNeighbours(Alloc& a,const Point& p,size_t k);

    const Point& point() const     { return value_.point();   }
    const Payload& payload() const { return value_.payload(); }

    template<typename ITER>
    static KDNode* build(Alloc& a,const ITER& begin, const ITER& end, int depth = 0);

    // For testing only

    NodeInfo nearestNeighbourBruteForce(Alloc& a,const Point& p);
    NodeList findInSphereBruteForce(Alloc& a,const Point& p, double radius);
    NodeList kNearestNeighboursBruteForce(Alloc& a,const Point& p,size_t k);

    // -------
    template<class Visitor>
    void visit(Alloc& a, Visitor& v, int depth = 0);

    // ---------
    void linkNodes(Alloc& a, KDNode*& prev = 0);


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
    KDNode* next(Alloc& a)  const { return a.convert(next_, this);   }

    void  left(Alloc& a,KDNode* n)  { left_  = a.convert(n); }
    void  right(Alloc& a,KDNode* n) { right_ = a.convert(n); }
    void  next(Alloc& a,KDNode* n)  { next_  = a.convert(n); }

    friend class KDTreeIterator<Traits>;

};

template<class Traits = KDMemory>
class KDTreeIterator {

    typedef typename Traits::Point   Point;
    typedef typename Traits::Payload Payload;
    typedef typename Traits::Alloc   Alloc;

    typedef typename Alloc::Ptr Ptr;
    typedef KDNode<Traits> Node;
    Alloc& alloc_;
    Ptr   ptr_;

public:
    KDTreeIterator(Alloc& alloc, Ptr ptr):
        alloc_(alloc), ptr_(ptr) {
        std::cout << "KDTreeIterator " << ptr << std::endl;
        Node* node = alloc_.convert(ptr_,(Node*)0);
        std::cout << "KDTreeIterator " << ptr << " " << node << std::endl;
        if(node) {
            if(!node->next(alloc_)) {
                Node* prev = 0;
                node->linkNodes(alloc, prev);
            }
        }
    }

    bool operator !=(const KDTreeIterator& other)
        { return ptr_ != other.ptr_; }

    operator const Point*() {
        return &(alloc_.convert(ptr_,(Node*)0)->point());
    }

    const Point* operator->() {
        return &(alloc_.convert(ptr_,(Node*)0)->point());
    }


    KDTreeIterator& operator++() {
        ptr_ = alloc_.convert(ptr_,(Node*)0)->next_;
        return *this;
    }
};


template<class Traits>
class KDTree {

public:
    typedef typename Traits::Point   Point;
    typedef typename Traits::Payload Payload;
    typedef typename Traits::Alloc   Alloc;

    typedef typename Alloc::Ptr Ptr;
    typedef KDNode<Traits> Node;
    typedef KDMetadata<Traits> Metadata;

    typedef          Point   PointType;
    typedef          Payload PayloadType;
    typedef typename KDNode<Traits>::NodeList NodeList;
    typedef          KDNodeInfo<Traits>       NodeInfo;
    typedef typename KDNode<Traits>::Value Value;

    Alloc alloc_;
    Ptr   root_;
    Metadata meta_;

    typedef KDTreeIterator<Traits> iterator;

    typedef std::pair<Point,Payload> value_type;

public:

    KDTree(const Alloc& alloc = Alloc()): alloc_(alloc), root_(alloc.root()) {}

    ~KDTree() {
        alloc_.deleteNode(root_,(Node*)0);
    }

    /// ITER must be a random access iterator
    /// WARNING: container is changed (sorted)
    template<typename ITER>
    void build(ITER begin, ITER end)
    {
        root_ = alloc_.convert(Node::build(alloc_, begin, end));
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
