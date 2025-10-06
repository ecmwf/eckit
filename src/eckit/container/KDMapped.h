/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef KDMapped_H
#define KDMapped_H

#include "eckit/container/StatCollector.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"

namespace eckit {

struct KDMappedHeader {
    size_t headerSize_;
    size_t itemCount_;
    size_t itemSize_;
    size_t metadataSize_;

    KDMappedHeader(size_t itemCount, size_t itemSize, size_t metadataSize) :
        headerSize_(sizeof(KDMappedHeader)), itemCount_(itemCount), itemSize_(itemSize), metadataSize_(metadataSize) {}
};


class KDMapped : public StatCollector {
public:

    KDMapped(const PathName&, size_t itemCount, size_t itemSize, size_t metadataSize);
    ~KDMapped();

    KDMapped(const KDMapped& other);
    KDMapped& operator=(const KDMapped& other);

    typedef size_t Ptr;

    template <class Node>
    Node* base(const Node*) {
        ASSERT(sizeof(Node) == header_.itemSize_);
        return reinterpret_cast<Node*>(base_);
    }

    Ptr root() const { return root_; }

    void root(Ptr r) { ASSERT(r == 1); }

    template <class Node>
    Ptr convert(Node* p) {
        return p ? p - base(p) : 0;
    }

    template <class Node>
    Node* convert(Ptr p, const Node* dummy) {
        Node* r = base(dummy);
        /* ASSERT(p < count_); */
        return p ? &r[p] : nullptr;
    }

    template <class Node, class A>
    Node* newNode1(const A& a, const Node* dummy) {
        Node* r = base(dummy);
        ASSERT(!readonly_);
        // ASSERT(count_ * sizeof(Node) < size_);
        return new (&r[++count_]) Node(a);
    }

    template <class Node, class A, class B>
    Node* newNode2(const A& a, const B& b, const Node* dummy) {
        Node* r = base(dummy);
        ASSERT(!readonly_);
        // ASSERT(count_ * sizeof(Node) < size_);
        return new (&r[++count_]) Node(a, b);
    }

    template <class Node, class A, class B, class C>
    Node* newNode3(const A& a, const B& b, const C& c, const Node* dummy) {
        Node* r = base(dummy);
        ASSERT(!readonly_);
        // ASSERT(count_ * sizeof(Node) < size_);
        return new (&r[++count_]) Node(a, b, c);
    }


    template <class Node>
    void deleteNode(Ptr p, Node* n) {
        // Ignore
        // TODO: recycle space if needed
    }

    void setMetadata(const void*, size_t);
    void getMetadata(void*, size_t);

    template <class Metadata>
    void setMetadata(const Metadata& meta) {
        setMetadata(&meta, sizeof(meta));
    }

    template <class Metadata>
    void getMetadata(Metadata& meta) {
        getMetadata(&meta, sizeof(meta));
    }

    size_t nbItems() const { return count_; }

private:

    PathName path_;

    KDMappedHeader header_;

    size_t count_{0};
    bool readonly_{true};

    long long size_;
    char* base_;
    Ptr root_;

    void* addr_;
    int fd_;
};

}  // namespace eckit


#endif
