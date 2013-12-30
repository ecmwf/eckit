/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef KDMapped_H
#define KDMapped_H

#include "eckit/filesystem/PathName.h"
#include "eckit/container/StatCollector.h"


namespace eckit {


class KDMapped : public StatCollector {
public:


    KDMapped(const PathName&,  size_t = 0, size_t metadata = 0);
    ~KDMapped();

    KDMapped(const KDMapped& other);
    KDMapped& operator=(const KDMapped& other);

    typedef size_t Ptr;

    template<class Node>
    Node* base(const Node*) {
        return reinterpret_cast<Node*>(reinterpret_cast<char*>(addr_) + header_ + metadata_);
    }

    template<class Node>
    Ptr convert(Node* p) {
        return p ? p - base(p) : 0; // FIXME: null and root are 0
    }

    template<class Node>
    Node* convert(Ptr p, const Node* dummy) {
        Node* r = base(dummy);
        /* ASSERT(p < count_); */
        return &r[p];
    }

    template<class Node, class A>
    Node* newNode1(const A& a, const Node* dummy) {
        Node* r = base(dummy);
        ASSERT(count_ * sizeof(Node) < size_);
        return new(&r[count_++]) Node(a);
    }

    template<class Node, class A, class B>
    Node* newNode2(const A& a, const B& b, const Node* dummy) {
        Node* r = base(dummy);
        ASSERT(count_ * sizeof(Node) < size_);
        return new(&r[count_++]) Node(a, b);
    }

    template<class Node, class A, class B, class C>
    Node* newNode3(const A& a, const B& b, const C& c, const Node* dummy) {
        Node* r = base(dummy);
        ASSERT(count_ * sizeof(Node) < size_);
        return new(&r[count_++]) Node(a, b, c);
    }


    template<class Node>
    void deleteNode(Ptr p, Node* n) {
        // Ignore
        // TODO: recycle space if needed
    }

    void setMetadata(const void*, size_t);
    void getMetadata(void*, size_t);

    template<class Metadata>
    void setMetadata(const Metadata& meta) {
        setMetadata(&meta, sizeof(meta));
    }

    template<class Metadata>
    void getMetadata(Metadata& meta) {
        getMetadata(&meta, sizeof(meta));
    }

private:
    PathName path_;
    size_t count_;
    size_t size_;
    size_t metadata_;
    size_t header_;

    void* addr_;
    int fd_;

};

} // Name space


#endif
