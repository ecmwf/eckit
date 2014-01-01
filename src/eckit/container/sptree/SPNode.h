/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef SPNode_H
#define SPNode_H

#include "eckit/container/sptree/SPNodeInfo.h"
#include "eckit/container/sptree/SPNodeQueue.h"
#include "eckit/container/sptree/SPValue.h"

namespace eckit {


template<class Traits>
class SPNode {
public:
    typedef typename Traits::Point   Point;
    typedef typename Traits::Payload Payload;
    typedef typename Traits::Alloc   Alloc;

    typedef SPNodeInfo<Traits>          NodeInfo;
    typedef typename NodeInfo::NodeList NodeList;
    typedef SPValue<Traits>             Value;
    typedef SPNodeQueue<Traits>         NodeQueue;

private:

    Value     value_;
    size_t    axis_;

    typedef typename Alloc::Ptr Ptr;
    Ptr left_;
    Ptr right_;
    Ptr next_; // For fast transversal

    friend struct SPMemory;


public:
    SPNode(const Value& value, size_t axis);

    NodeInfo nearestNeighbour(Alloc& a,const Point& p);
    NodeList findInSphere(Alloc& a,const Point& p, double radius);
    NodeList kNearestNeighbours(Alloc& a,const Point& p,size_t k);

    const Point& point() const     { return value_.point();   }
    const Payload& payload() const { return value_.payload(); }
    Value& value()     { return value_;   }
    const Value& value() const    { return value_;   }

    template<typename ITER>
    static SPNode* build(Alloc& a,const ITER& begin, const ITER& end, int depth = 0);

    // For testing only

    NodeInfo nearestNeighbourBruteForce(Alloc& a,const Point& p);
    NodeList findInSphereBruteForce(Alloc& a,const Point& p, double radius);
    NodeList kNearestNeighboursBruteForce(Alloc& a,const Point& p,size_t k);

    // -------
    template<class Visitor>
    void visit(Alloc& a, Visitor& v, int depth = 0);

    // ---------
    void linkNodes(Alloc& a, SPNode*& prev = 0);


private:
    void nearestNeighbour(Alloc& a,const Point& p, SPNode*& best, double& max, int depth);
    void nearestNeighbourBruteForce(Alloc& a,const Point& p,SPNode*& best,double& max, int depth);
    void findInSphere(Alloc& a,const Point& p ,double radius, NodeList& result, int depth) ;
    void findInSphereBruteForce(Alloc& a,const Point& p, double radius, NodeList& result, int depth) ;
    void kNearestNeighbours(Alloc& a,const Point& p ,size_t k, NodeQueue& result, int depth) ;
    void kNearestNeighboursBruteForce(Alloc& a,const Point& p, size_t k, NodeQueue& result, int depth) ;

    //==========================

    SPNode* left(Alloc& a)  const { return a.convert(left_, this);   }
    SPNode* right(Alloc& a) const { return a.convert(right_, this);  }
    SPNode* next(Alloc& a)  const { return a.convert(next_, this);   }

    void  left(Alloc& a,SPNode* n)  { left_  = a.convert(n); }
    void  right(Alloc& a,SPNode* n) { right_ = a.convert(n); }
    void  next(Alloc& a,SPNode* n)  { next_  = a.convert(n); }

    friend class SPTreeIterator<Traits>;

};


} // Name space

#include "SPNode.cc"

#endif
