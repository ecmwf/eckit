/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef SPNode_H
#define SPNode_H

#include "eckit/eckit.h"

#include "eckit/container/sptree/SPNodeInfo.h"
#include "eckit/container/sptree/SPNodeQueue.h"
#include "eckit/container/sptree/SPValue.h"

namespace eckit {


template <class Traits, class NodeType>
class SPNode {
public:

    typedef typename Traits::Point Point;
    typedef typename Traits::Payload Payload;
    typedef typename Traits::Alloc Alloc;

    typedef SPNodeInfo<Traits, NodeType> NodeInfo;
    typedef typename NodeInfo::NodeList NodeList;
    typedef SPValue<Traits> Value;
    typedef SPNodeQueue<Traits, NodeType> NodeQueue;

    typedef NodeType Node;

protected:

    Value value_;

    typedef typename Alloc::Ptr Ptr;
    Ptr left_;
    Ptr right_;
    Ptr next_;  // For fast transversal

    friend struct SPMemory;


public:

    // SPNode(const Value& value);

    template <class V>
    SPNode(const V& value);

    ~SPNode() {}

    NodeInfo nearestNeighbour(Alloc& a, const Point& p);
    NodeList findInSphere(Alloc& a, const Point& p, double radius);
    NodeList kNearestNeighbours(Alloc& a, const Point& p, size_t k);

    const Point& point() const { return value_.point(); }
    const Payload& payload() const { return value_.payload(); }
    Value& value() { return value_; }
    const Value& value() const { return value_; }

    template <typename ITER>
    static SPNode* build(Alloc& a, const ITER& begin, const ITER& end, int depth = 0);

    // For testing only

    NodeInfo nearestNeighbourBruteForce(Alloc& a, const Point& p);
    NodeList findInSphereBruteForce(Alloc& a, const Point& p, double radius);
    NodeList kNearestNeighboursBruteForce(Alloc& a, const Point& p, size_t k);

    // -------
    template <class Visitor>
    void visit(Alloc& a, Visitor& v, int depth = 0);

    // ---------
    void linkNodes(Alloc& a, Node*& prev = 0);

    const Node* asNode() const { return static_cast<const Node*>(this); }
    Node* asNode() { return static_cast<Node*>(this); }

public:  // because of a clang bug. Should be protected

    // void nearestNeighbourX(Alloc& a,const Point& p, Node*& best, double& max, int depth) = 0;
    void nearestNeighbourBruteForceX(Alloc& a, const Point& p, Node*& best, double& max, int depth);
    // void findInSphereX(Alloc& a,const Point& p ,double radius, NodeList& result, int depth) = 0;
    void findInSphereBruteForceX(Alloc& a, const Point& p, double radius, NodeList& result, int depth);
    // void kNearestNeighboursX(Alloc& a,const Point& p ,size_t k, NodeQueue& result, int depth) = 0;
    void kNearestNeighboursBruteForceX(Alloc& a, const Point& p, size_t k, NodeQueue& result, int depth);

    //==========================

public:

    Node* left(Alloc& a) const { return a.convert(left_, (Node*)0); }
    Node* right(Alloc& a) const { return a.convert(right_, (Node*)0); }
    Node* next(Alloc& a) const { return a.convert(next_, (Node*)0); }

    void left(Alloc& a, Node* n) { left_ = a.convert(n); }
    void right(Alloc& a, Node* n) { right_ = a.convert(n); }
    void next(Alloc& a, Node* n) { next_ = a.convert(n); }

    friend class SPIterator<Traits, NodeType>;
};


}  // namespace eckit

#include "SPNode.cc"

#endif
