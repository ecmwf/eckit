// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/search/SharedMemory.h"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <ostream>
#include <set>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/log/Log.h"
#include "eckit/memory/Shmget.h"
#include "eckit/os/Stat.h"


namespace eckit::geo::search {


static const KDTreeBuilder<SharedMemory> BUILDER_1("shmem");
static const KDTreeBuilder<SharedMemory> BUILDER_2("shared-memory");
static const KDTreeBuilder<TempSharedMemory> BUILDER_TEMP_1("tmp-shmem");
static const KDTreeBuilder<TempSharedMemory> BUILDER_TEMP_2("tmp-shared-memory");


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


size_t segment_size(size_t file_size) {
    const auto page = static_cast<size_t>(::sysconf(_SC_PAGESIZE));
    return ((INFO_SIZE + file_size + page - 1) / page) * page;
}


key_t shared_memory_key(const PathName& path) {
    Stat::Struct s;
    SYSCALL(Stat::stat(path.localPath(), &s));

    // a rebuilt file (inode, modification time) is a different k-d tree, so it maps to a different segment
    const auto key = ::ftok(path.localPath(), static_cast<int>(s.st_mtime % 255) + 1);
    if (key == static_cast<key_t>(-1)) {
        throw FailedSystemCall("ftok(" + path.asString() + ")");
    }

    return key;
}


/// Removes shared memory segments at exit, once detached (processes attached to them are unaffected)
class Unloader {
public:

    static Unloader& instance() {
        static Unloader unloader;
        return unloader;
    }

    void add(int shmid) {
        util::lock_guard<util::recursive_mutex> lock(mutex_);
        shmids_.insert(shmid);
    }

    Unloader(const Unloader&)            = delete;
    Unloader(Unloader&&)                 = delete;
    Unloader& operator=(const Unloader&) = delete;
    Unloader& operator=(Unloader&&)      = delete;

    ~Unloader() {
        for (auto shmid : shmids_) {
            ::shmctl(shmid, IPC_RMID, nullptr);  // failing if removed already (e.g. by another process)
        }
    }

private:

    Unloader() = default;

    util::recursive_mutex mutex_;
    std::set<int> shmids_;
};


}  // namespace


struct SharedMemory::Segment {
    Segment(void* _address, size_t _size) : address(_address), size(_size) {}

    Segment(const Segment&)            = delete;
    Segment(Segment&&)                 = delete;
    Segment& operator=(const Segment&) = delete;
    Segment& operator=(Segment&&)      = delete;

    ~Segment() { Shmget::shmdt(address); }

    char* content() const { return static_cast<char*>(address) + INFO_SIZE; }

    void* const address;
    const size_t size;
};


SharedMemory::SharedMemory(const std::string& uid, size_t size) : SharedMemory(uid, size, false) {}


SharedMemory::SharedMemory(const std::string& uid, size_t size, bool unload_at_exit) :
    LoadedFile(uid, size), unload_at_exit_(unload_at_exit) {}


SharedMemory::~SharedMemory() = default;


KDTree::MemoryUsage SharedMemory::footprint() const {
    return loaded() ? MemoryUsage{0, segment_->size} : LoadedFile::footprint();
}


KDTree::MemoryUsage SharedMemory::estimate(size_t size) {
    return {0, segment_size(storage_size(size))};
}


bool SharedMemory::is_loaded(const PathName& path) {
    // the segment key is of the (existing) cache file
    return path.exists() && ::shmget(shared_memory_key(path.realName()), 0, 0) >= 0;
}


void SharedMemory::unload(const PathName& path) {
    if (!path.exists()) {
        return;
    }

    if (auto shmid = ::shmget(shared_memory_key(path.realName()), 0, 0); shmid >= 0) {
        SYSCALL(::shmctl(shmid, IPC_RMID, nullptr));
    }
}


char* SharedMemory::load() {
    try {
        attach();
        return segment_->content();
    }
    catch (const std::exception& e) {
        Log::warning() << "SharedMemory: shared memory not available, using '" << path() << "': " << e.what()
                       << std::endl;
        return nullptr;
    }
}


void SharedMemory::attach() {
    ASSERT(!segment_);

    const auto real      = path().realName();
    const auto name      = real.asString();
    const auto file_size = static_cast<size_t>(real.size());

    if (name.size() >= sizeof(Info::path)) {
        throw exception::SearchError("SharedMemory: path too long '" + name + "'", Here());
    }

    const auto size  = segment_size(file_size);
    const auto shmid = Shmget::shmget(shared_memory_key(real), size, IPC_CREAT | 0600);
    if (shmid < 0) {
        throw FailedSystemCall("shmget, check system limits (Linux: ipcs -l, macOS: ipcs -M)");
    }

    auto* address = Shmget::shmat(shmid, nullptr, 0);
    if (address == reinterpret_cast<void*>(-1)) {
        throw FailedSystemCall("shmat");
    }

    auto segment = std::make_unique<Segment>(address, size);
    auto* info   = static_cast<Info*>(address);

    // new segments are zero-initialised, and loading is exclusive (under lock, as building)
    if (info->ready == 0) {
        if (std::ifstream in(name, std::ios::binary);
            !in.read(segment->content(), static_cast<std::streamsize>(file_size))) {
            throw exception::SearchError("SharedMemory: failed to read '" + name + "'", Here());
        }

        info->magic = MAGIC;
        info->size  = file_size;
        std::strncpy(info->path, name.c_str(), sizeof(info->path) - 1);
        info->ready = 1;
    }
    else if (info->magic != MAGIC || info->size != file_size || name != info->path) {
        throw exception::SearchError("SharedMemory: segment does not hold '" + name + "'", Here());
    }

    if (unload_at_exit_) {
        Unloader::instance().add(shmid);
    }

    segment_ = std::move(segment);
}


void SharedMemory::print(std::ostream& out) const {
    out << "SharedMemory[path=" << path() << ",size=" << size() << ",loaded=" << loaded()
        << ",unload_at_exit=" << unload_at_exit_ << "]";
}


TempSharedMemory::TempSharedMemory(const std::string& uid, size_t size) : SharedMemory(uid, size, true) {}


}  // namespace eckit::geo::search
