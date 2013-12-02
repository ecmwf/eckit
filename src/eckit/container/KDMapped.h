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


namespace eckit {


class KDMapped {
public:


    KDMapped(const PathName&, size_t);
    ~KDMapped();

    KDMapped(const KDMapped& other);
    KDMapped& operator=(const KDMapped& other);

    typedef size_t Ptr;

    template<class Node>
    Ptr convert(Node* p) {
        Node* base = static_cast<Node*>(addr_);
        return p ? p - base : 0; // FIXME: null and root are 0
    }

    template<class Node>
    Node* convert(Ptr p, const Node*) {
        Node* base = static_cast<Node*>(addr_);
        /* ASSERT(p < count_); */
        return &base[p];
    }

    template<class Node, class Init>
    Node* newNode(const Init& init, const Node*) {
        Node* base = static_cast<Node*>(addr_);
        return new(&base[count_++]) Node(init);
    }

    template<class Node>
    void deleteNode(Ptr p, Node* n) {
        // Ignore
        // TODO: recycle space if needed
    }

private:
    PathName path_;
    size_t count_;
    size_t size_;

    void* addr_;
    int fd_;

};

} // Name space


#endif
