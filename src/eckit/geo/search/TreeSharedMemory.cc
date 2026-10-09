// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/search/TreeSharedMemory.h"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <ostream>

#include "eckit/container/KDMapped.h"
#include "eckit/container/KDTree.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/memory/Shmget.h"
#include "eckit/os/Stat.h"


namespace eckit::geo::search {


static const TreeRegisterType<TreeSharedMemory> BUILDER("shared-memory");


namespace {


constexpr int MAGIC = 0x6b647472;


struct Info {
    int magic;
    int ready;
    size_t size;
    char path[1024];
};


// the content (cache file) follows, aligned beyond any node alignment
constexpr size_t INFO_SIZE = ((sizeof(Info) + 63) / 64) * 64;


key_t shared_memory_key(const PathName& path) {
    Stat::Struct s;
    SYSCALL(Stat::stat(path.localPath(), &s));

    // a rebuilt file (inode, modification time) is a different tree, so it maps to a different segment
    const auto key = ::ftok(path.localPath(), static_cast<int>(s.st_mtime % 255) + 1);
    if (key == static_cast<key_t>(-1)) {
        throw FailedSystemCall("ftok(" + path.asString() + ")");
    }
    return key;
}


/// Read-only k-d tree allocator, over memory holding a KDMapped tree
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

    template <class Node>
    void deleteNode(Ptr, Node*) {}

    size_t nbItems() const { return count_; }

private:

    char* base_;
    size_t count_;
};


using SharedKDTree = KDTreeX<TT<Traits, Alloc>>;

static_assert(sizeof(SharedKDTree::Node) == sizeof(KDTreeMapped<Traits>::Node), "k-d tree node layouts differ");


}  // namespace


struct TreeSharedMemory::Segment {
    Segment(void* _address, size_t _size, const KDMappedHeader& h) :
        address(_address),
        size(_size),
        alloc(static_cast<char*>(address) + INFO_SIZE +
                  ((h.headerSize_ + h.metadataSize_ + h.itemSize_ - 1) / h.itemSize_) * h.itemSize_,
              h.itemCount_),
        tree(alloc) {}

    Segment(const Segment&)            = delete;
    Segment(Segment&&)                 = delete;
    Segment& operator=(const Segment&) = delete;
    Segment& operator=(Segment&&)      = delete;

    ~Segment() { Shmget::shmdt(address); }

    void* address;
    size_t size;
    Alloc alloc;
    SharedKDTree tree;
};


TreeSharedMemory::TreeSharedMemory(const std::string& uid, size_t size) :
    TreeMappedFile(uid, size, TreeMappedCacheFile::roots()) {}


TreeSharedMemory::~TreeSharedMemory() = default;


Neighbour TreeSharedMemory::nearest_neighbour(const Point& p) {
    return segment_ ? to_neighbour(segment_->tree.nearestNeighbour(p)) : TreeMappedFile::nearest_neighbour(p);
}


Neighbours TreeSharedMemory::k_nearest_neighbours(const Point& p, size_t k) {
    return segment_ ? to_neighbours(segment_->tree.kNearestNeighbours(p, k))
                    : TreeMappedFile::k_nearest_neighbours(p, k);
}


Neighbours TreeSharedMemory::find_in_sphere(const Point& p, double radius) {
    return segment_ ? to_neighbours(segment_->tree.findInSphere(p, radius)) : TreeMappedFile::find_in_sphere(p, radius);
}


bool TreeSharedMemory::ready() {
    if (segment_) {
        return true;
    }

    if (!TreeMappedFile::ready()) {
        return false;
    }

    load();
    return true;
}


void TreeSharedMemory::commit() {
    TreeMappedFile::commit();
    load();
}


void TreeSharedMemory::stats_print(std::ostream& out, bool pretty) const {
    if (segment_) {
        segment_->tree.statsPrint(out, pretty);
        return;
    }
    TreeMappedFile::stats_print(out, pretty);
}


void TreeSharedMemory::stats_reset() {
    if (segment_) {
        segment_->tree.statsReset();
        return;
    }
    TreeMappedFile::stats_reset();
}


Tree::MemoryUsage TreeSharedMemory::footprint() const {
    return segment_ ? MemoryUsage{0, segment_->size} : TreeMappedFile::footprint();
}


void TreeSharedMemory::unload(const PathName& path) {
    const auto real = path.realName();
    if (!real.exists()) {
        return;
    }

    if (auto shmid = ::shmget(shared_memory_key(real), 0, 0); shmid >= 0) {
        SYSCALL(::shmctl(shmid, IPC_RMID, nullptr));
    }
}


void TreeSharedMemory::load() {
    try {
        attach();
        close();  // the file mapping is no longer required
    }
    catch (const std::exception& e) {
        Log::warning() << "TreeSharedMemory: shared memory not available, using '" << path() << "': " << e.what()
                       << std::endl;
    }
}


void TreeSharedMemory::attach() {
    const auto real      = path().realName();
    const auto name      = real.asString();
    const auto file_size = static_cast<size_t>(real.size());

    if (name.size() >= sizeof(Info::path)) {
        throw exception::SearchError("TreeSharedMemory: path too long '" + name + "'", Here());
    }

    const auto page = static_cast<size_t>(::sysconf(_SC_PAGESIZE));
    const auto size = ((INFO_SIZE + file_size + page - 1) / page) * page;

    const auto shmid = Shmget::shmget(shared_memory_key(real), size, IPC_CREAT | 0600);
    if (shmid < 0) {
        throw FailedSystemCall("shmget, check system limits (Linux: ipcs -l, macOS: ipcs -M)");
    }

    auto* address = Shmget::shmat(shmid, nullptr, 0);
    if (address == reinterpret_cast<void*>(-1)) {
        throw FailedSystemCall("shmat");
    }

    auto* info    = static_cast<Info*>(address);
    auto* content = static_cast<char*>(address) + INFO_SIZE;

    // new segments are zero-initialised, and loading is exclusive (under lock, as building)
    if (info->ready == 0) {
        if (std::ifstream in(name, std::ios::binary); !in.read(content, static_cast<std::streamsize>(file_size))) {
            Shmget::shmdt(address);
            throw exception::SearchError("TreeSharedMemory: failed to read '" + name + "'", Here());
        }

        info->magic = MAGIC;
        info->size  = file_size;
        std::strncpy(info->path, name.c_str(), sizeof(info->path) - 1);
        info->ready = 1;
    }
    else if (info->magic != MAGIC || info->size != file_size || name != info->path) {
        Shmget::shmdt(address);
        throw exception::SearchError("TreeSharedMemory: segment does not hold '" + name + "'", Here());
    }

    KDMappedHeader header(0, 0, 0);
    std::memcpy(&header, content, sizeof(header));

    segment_ = std::make_unique<Segment>(address, size, header);
}


void TreeSharedMemory::print(std::ostream& out) const {
    out << "TreeSharedMemory[path=" << path() << ",size=" << size() << ",shared_memory=" << in_shared_memory() << "]";
}


}  // namespace eckit::geo::search
