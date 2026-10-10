// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/search/LoadedFile.h"

#include <cstddef>
#include <cstring>

#include "eckit/container/KDMapped.h"
#include "eckit/container/KDTree.h"
#include "eckit/exception/Exceptions.h"


namespace eckit::geo::search {


namespace {


/// Read-only allocator, over memory holding a k-d tree of the KDMapped layout (as its cache file)
struct Alloc : StatCollector {
    using Ptr = size_t;

    Alloc(char* base, size_t count) : base_(base), count_(count) {}

    Ptr root() const { return 1; }
    void root(Ptr) { NOTIMP; }

    template <class Node>
    Ptr convert(Node* p) {
        return p != nullptr ? static_cast<Ptr>(p - reinterpret_cast<Node*>(base_)) : 0;
    }

    template <class Node>
    Node* convert(Ptr p, const Node*) {
        return p != 0 ? reinterpret_cast<Node*>(base_) + p : nullptr;
    }

    template <class Node, class A>
    Node* newNode1(const A&, const Node*) {
        NOTIMP;
    }

    template <class Node, class A, class B>
    Node* newNode2(const A&, const B&, const Node*) {
        NOTIMP;
    }

    template <class Node, class A, class B, class C>
    Node* newNode3(const A&, const B&, const C&, const Node*) {
        NOTIMP;
    }

    // nodes are not owned, so the k-d tree destruction doesn't access them
    template <class Node>
    void deleteNode(Ptr, Node*) {}

    size_t nbItems() const { return count_; }

private:

    char* base_;
    size_t count_;
};


using ReadOnlyKDTree = KDTreeX<TT<Traits, Alloc>>;

static_assert(sizeof(ReadOnlyKDTree::Node) == sizeof(KDTreeMapped<Traits>::Node), "k-d tree node layouts differ");

// loaded content is aligned at least as allocated (operator new, shared memory pages)
static_assert(alignof(ReadOnlyKDTree::Node) <= alignof(std::max_align_t), "k-d tree node alignment");


KDMappedHeader header(const char* content) {
    KDMappedHeader h(0, 0, 0);
    std::memcpy(&h, content, sizeof(h));
    return h;
}


// as KDMapped: items follow the header and metadata, aligned to the item size
char* items(char* content) {
    const auto h = header(content);
    return content + ((h.headerSize_ + h.metadataSize_ + h.itemSize_ - 1) / h.itemSize_) * h.itemSize_;
}


}  // namespace


struct LoadedFile::Content {
    explicit Content(char* content) : alloc(items(content), header(content).itemCount_), kdtree(alloc) {}

    Alloc alloc;
    ReadOnlyKDTree kdtree;
};


LoadedFile::LoadedFile(const std::string& uid, size_t size) : MappedFile(uid, size) {}


LoadedFile::~LoadedFile() = default;


Neighbour LoadedFile::nearest_neighbour(const Point& p) {
    return content_ ? to_neighbour(content_->kdtree.nearestNeighbour(p)) : MappedFile::nearest_neighbour(p);
}


Neighbours LoadedFile::k_nearest_neighbours(const Point& p, size_t k) {
    return content_ ? to_neighbours(content_->kdtree.kNearestNeighbours(p, k)) : MappedFile::k_nearest_neighbours(p, k);
}


Neighbours LoadedFile::find_in_sphere(const Point& p, double radius) {
    return content_ ? to_neighbours(content_->kdtree.findInSphere(p, radius)) : MappedFile::find_in_sphere(p, radius);
}


bool LoadedFile::ready() {
    if (content_) {
        return true;
    }

    if (!MappedFile::ready()) {
        return false;
    }

    load_content();
    return true;
}


void LoadedFile::commit() {
    MappedFile::commit();
    load_content();
}


void LoadedFile::stats_print(std::ostream& out, bool pretty) const {
    if (content_) {
        content_->kdtree.statsPrint(out, pretty);
        return;
    }

    MappedFile::stats_print(out, pretty);
}


void LoadedFile::stats_reset() {
    if (content_) {
        content_->kdtree.statsReset();
        return;
    }

    MappedFile::stats_reset();
}


void LoadedFile::load_content() {
    ASSERT(!content_);

    if (auto* content = load(); content != nullptr) {
        content_ = std::make_unique<Content>(content);
        close();  // the cache file mapping is no longer required
    }
}


}  // namespace eckit::geo::search
