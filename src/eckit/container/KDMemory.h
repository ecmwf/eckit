/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef KDMemory_H
#define KDMemory_H

#include "eckit/eckit.h"

//#include <cmath>
#include <limits>
#include <cmath>

#include "eckit/container/StatCollector.h"

namespace eckit {

struct KDMemory : public StatCollector {
    typedef void* Ptr;

    Ptr root() const { return 0; }
    void root(Ptr r) {}

    template<class Node>
    Ptr convert(Node* p) { return p; }

    template<class Node>
    Node* convert(Ptr p,  const Node*) { return static_cast<Node*>(p); }

    template<class Node,typename A>
    Node* newNode1(const A& a, const Node*) { return new Node(a); }

    template<class Node,typename A, typename B>
    Node* newNode2( const A& a,  const B& b, const Node*) { return new Node(a, b); }

    template<class Node,typename A, typename B, typename C>
    Node* newNode3( const A& a,  const B& b,  const C& c, const Node*) { return new Node(a, b, c); }

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

template<class T, class A>
struct TT : public T {
    typedef A Alloc;
};

} // end namespace


#endif
