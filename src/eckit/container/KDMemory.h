// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef KDMemory_H
#define KDMemory_H

#include "eckit/eckit.h"

#include <cmath>
#include <limits>

#include "eckit/container/StatCollector.h"

//------------------------------------------------------------------------------------------------------

namespace eckit {

//------------------------------------------------------------------------------------------------------

struct KDMemory : public StatCollector {
    using Ptr = void*;

    Ptr root() const { return nullptr; }
    void root(Ptr) {}

    template <class Node>
    Ptr convert(Node* p) {
        return p;
    }

    template <class Node>
    Node* convert(Ptr p, const Node*) {
        return static_cast<Node*>(p);
    }

    template <class Node, typename A>
    Node* newNode1(const A& a, const Node*) {
        nbItems_++;
        return new Node(a);
    }

    template <class Node, typename A, typename B>
    Node* newNode2(const A& a, const B& b, const Node*) {
        nbItems_++;
        return new Node(a, b);
    }

    template <class Node, typename A, typename B, typename C>
    Node* newNode3(const A& a, const B& b, const C& c, const Node*) {
        nbItems_++;
        return new Node(a, b, c);
    }

    template <class Node>
    void deleteNode(Ptr p, const Node*) {
        Node* n = static_cast<Node*>(p);
        if (n) {
            deleteNode(n->left(*this), n);
            deleteNode(n->right(*this), n);
            delete n;
            nbItems_--;
        }
    }

    size_t nbItems() const { return nbItems_; }

private:

    size_t nbItems_{0};
};

template <class T, class A>
struct TT : public T {
    using Alloc = A;
};

//------------------------------------------------------------------------------------------------------

}  // end namespace eckit

#endif
