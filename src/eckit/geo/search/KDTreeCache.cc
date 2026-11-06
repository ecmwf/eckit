// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/search/KDTreeCache.h"


#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <ostream>

#include "eckit/cache/FileCache.h"
#include "eckit/cache/Loader.h"
#include "eckit/container/KDMapped.h"
#include "eckit/container/KDTree.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/cache/SearchCache.h"
#include "eckit/log/Log.h"


namespace eckit::geo::search {


namespace {


using MappedContainer = KDTreeMapped<Traits>;


/// Read-only allocator over memory holding a k-d tree in the KDMapped layout
struct Alloc : StatCollector {
    using Ptr = size_t;

    Alloc(const char* base, size_t count) : base_(base), count_(count) {}

    Ptr root() const { return 1; }
    void root(Ptr) { NOTIMP; }

    template <class Node>
    Ptr convert(Node* p) {
        return p != nullptr ? static_cast<Ptr>(p - reinterpret_cast<const Node*>(base_)) : 0;
    }

    template <class Node>
    Node* convert(Ptr p, const Node*) {
        if (p > count_) {
            throw exception::SearchError(
                "KDTreeCache: corrupt k-d tree (node " + std::to_string(p) + " of " + std::to_string(count_) + ")",
                Here());
        }

        // queries don't modify nodes (the content is read-only)
        return p != 0 ? const_cast<Node*>(reinterpret_cast<const Node*>(base_)) + p : nullptr;
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

    // nodes are not owned, so destruction doesn't access them
    template <class Node>
    void deleteNode(Ptr, Node*) {}

    size_t nbItems() const { return count_; }

private:

    const char* base_;
    size_t count_;
};


using ReadOnlyKDTree = KDTreeX<TT<Traits, Alloc>>;

static_assert(sizeof(ReadOnlyKDTree::Node) == sizeof(MappedContainer::Node), "k-d tree node layouts differ");

// loaded content is aligned at least as allocated (operator new, memory mapping, shared memory pages)
static_assert(alignof(ReadOnlyKDTree::Node) <= alignof(std::max_align_t), "k-d tree node alignment");


KDMappedHeader header(const char* content) {
    KDMappedHeader h(0, 0, 0);
    std::memcpy(&h, content, sizeof(h));
    return h;
}


// as KDMapped: items follow the header and metadata, aligned to the item size
const char* items(const char* content) {
    const auto h = header(content);
    return content + ((h.headerSize_ + h.metadataSize_ + h.itemSize_ - 1) / h.itemSize_) * h.itemSize_;
}


// uid is part of a path, restrict to safe characters
bool safe_uid(const std::string& uid) {
    return !uid.empty() &&
           std::all_of(uid.begin(), uid.end(), [](unsigned char c) { return std::isalnum(c) || c == '-' || c == '_'; });
}


}  // namespace


struct KDTreeCache::Content {
    explicit Content(const char* content) : alloc(items(content), header(content).itemCount_), kdtree(alloc) {}

    Alloc alloc;
    ReadOnlyKDTree kdtree;
};


KDTreeCache::KDTreeCache(const std::string& loader, const std::string& uid, size_t size) :
    KDTree(size), loader_name_(loader), uid_(uid) {
    if (!loads(loader_name_)) {
        throw exception::SearchError("KDTreeCache: loader '" + loader_name_ + "' doesn't load k-d tree cache files",
                                     Here());
    }

    // uid is part of the cache file path
    if (!safe_uid(uid_)) {
        throw exception::SearchError("KDTreeCache: invalid uid '" + uid_ + "'", Here());
    }
}


KDTreeCache::~KDTreeCache() = default;


const PathName& KDTreeCache::path() const {
    if (!content_) {
        throw exception::SearchError("KDTreeCache: not built", Here());
    }
    return path_;
}


bool KDTreeCache::found() const {
    if (!found_) {
        found_ =
            cache::SearchCache::file_cache().find(uid_, path_, [this](const PathName& p) { return valid(p, size()); });
    }
    return found_;
}


const eckit::cache::Loader& KDTreeCache::loader() const {
    if (!loader_) {
        throw exception::SearchError("KDTreeCache: not built", Here());
    }
    return *loader_;
}


bool KDTreeCache::build(const Fill& fill, bool balanced) {
    if (content_) {
        return false;
    }

    bool built = false;
    if (!found()) {
        path_ = cache::SearchCache::file_cache().get_or_create(
            uid_,
            [this, &fill, balanced](const PathName& tmp) {
                auto values = this->values(fill);

                MappedContainer tree(tmp, size(), 0);

                if (balanced) {
                    // native value type, avoiding conversions while partitioning
                    std::vector<MappedContainer::Value> v(values.begin(), values.end());
                    tree.build(v);
                }
                else {
                    for (const auto& value : values) {
                        tree.insert(value);
                    }
                }
            },
            [this](const PathName& path) { return valid(path, size()); }, &built);
    }

    loader_.reset(cache::SearchCache::file_cache().load(loader_name_, path_));

    // as the cache file (e.g. not replaced since checked)
    const auto* content = static_cast<const char*>(loader_->address());
    if (!valid(content, loader_->size(), size())) {
        loader_.reset();
        throw exception::SearchError("KDTreeCache: '" + path_.asString() + "' invalid once loaded", Here());
    }

    content_ = std::make_unique<Content>(content);

    return built;
}


Neighbour KDTreeCache::nearest_neighbour(const Point& p) {
    return to_neighbour(content().kdtree.nearestNeighbour(p));
}


Neighbours KDTreeCache::k_nearest_neighbours(const Point& p, size_t k) {
    return to_neighbours(content().kdtree.kNearestNeighbours(p, k));
}


Neighbours KDTreeCache::find_in_sphere(const Point& p, double radius) {
    return to_neighbours(content().kdtree.findInSphere(p, radius));
}


void KDTreeCache::stats_print(std::ostream& out, bool pretty) const {
    content().kdtree.statsPrint(out, pretty);
}


void KDTreeCache::stats_reset() {
    content().kdtree.statsReset();
}


KDTree::MemoryUsage KDTreeCache::footprint() const {
    return loader_ ? loader_->footprint() : MemoryUsage{};
}


bool KDTreeCache::loads(const std::string& loader) {
    return loader != "memory" && loader != "stream" && eckit::cache::LoaderFactory::has(loader);
}


KDTree::MemoryUsage KDTreeCache::estimate(const std::string& loader, size_t size) {
    return eckit::cache::LoaderFactory::estimate(loader, storage_size(size));
}


size_t KDTreeCache::storage_size(size_t size) {
    constexpr size_t item = sizeof(MappedContainer::Node);

    // as KDMapped: header (no metadata) aligned to item size, then (unused) item 0 and items
    constexpr size_t base = ((sizeof(KDMappedHeader) + item - 1) / item) * item;
    return base + (size + 1) * item;
}


bool KDTreeCache::valid(const PathName& path, size_t size) {
    std::ifstream in(path.localPath(), std::ios::binary);

    char content[sizeof(KDMappedHeader)];
    return in.read(content, sizeof(content)) && valid(content, static_cast<size_t>(path.size()), size);
}


bool KDTreeCache::valid(const char* content, size_t content_size, size_t size) {
    if (content == nullptr || content_size != storage_size(size)) {
        return false;
    }

    const auto h = header(content);
    return h.headerSize_ == sizeof(KDMappedHeader) && h.itemSize_ == sizeof(MappedContainer::Node) &&
           h.itemCount_ == size && h.metadataSize_ == 0;
}


KDTreeCache::Content& KDTreeCache::content() {
    return const_cast<Content&>(static_cast<const KDTreeCache&>(*this).content());
}


const KDTreeCache::Content& KDTreeCache::content() const {
    if (!content_) {
        throw exception::SearchError("KDTreeCache: not built", Here());
    }
    return *content_;
}


void KDTreeCache::print(std::ostream& out) const {
    out << "KDTreeCache[loader=" << loader_name_
        << ",path=" << (content_ ? path_ : cache::SearchCache::file_cache().relative(uid_)) << ",size=" << size()
        << "]";
}


}  // namespace eckit::geo::search
