// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/loader/SharedMemory.h"

#include <signal.h>
#include <sys/ipc.h>
#include <unistd.h>

#include <atomic>
#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <ostream>
#include <set>
#include <sstream>
#include <thread>

#include "eckit/cache/Exceptions.h"
#include "eckit/cache/LibEcKitCache.h"
#include "eckit/cache/PathLock.h"
#include "eckit/cache/Shmget.h"
#include "eckit/cache/mutex.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Log.h"
#include "eckit/log/Timer.h"
#include "eckit/os/Stat.h"


namespace eckit::cache::loader {


static const LoaderBuilder<SharedMemory> BUILDER_1("shared-memory");
static const LoaderBuilder<SharedMemory> BUILDER_2("shmem");
static const LoaderBuilder<TempSharedMemory> BUILDER_TEMP_1("tmp-shared-memory");
static const LoaderBuilder<TempSharedMemory> BUILDER_TEMP_2("tmp-shmem");


namespace {


/**
 * A file's identity (device, inode) and version (mtime, size); the segment key alone can't tell, as it uses only a few
 * bits of inode and mtime, so different files or versions can share a key
 */
struct Version {
    std::uint64_t dev     = 0;
    std::uint64_t ino     = 0;
    std::int64_t mtime_s  = 0;
    std::int64_t mtime_ns = 0;
    std::uint64_t size    = 0;

    bool operator==(const Version& other) const {
        return dev == other.dev && ino == other.ino && mtime_s == other.mtime_s && mtime_ns == other.mtime_ns &&
               size == other.size;
    }
};


/// A file as it is now: real path, version and segment key, from one stat
struct Source {
    std::string path;
    Version version;
    key_t key;

    static Source of(const PathName& path) {
        const auto real = path.realName();

        Stat::Struct s;
        SYSCALL(Stat::stat(real.localPath(), &s));

#if defined(__APPLE__)
        const auto mtime_ns = s.st_mtimespec.tv_nsec;
#else
        const auto mtime_ns = s.st_mtim.tv_nsec;
#endif

        return {real.asString(),
                {static_cast<std::uint64_t>(s.st_dev), static_cast<std::uint64_t>(s.st_ino),
                 static_cast<std::int64_t>(s.st_mtime), static_cast<std::int64_t>(mtime_ns),
                 static_cast<std::uint64_t>(s.st_size)},
                calculate_shmget_key_from_path(real, s.st_mtime)};
    }
};


/**
 * Segment header, checked by attaching processes: file (path, Version) and kind of content (magic). magic is first,
 * unlike mir's segments, so those are never misread. ready is stored last (release) and loaded first (acquire), so the
 * content is complete even without the lock, on any memory model.
 */
struct Info {
    int magic;
    std::atomic<int> ready;
    Version version;
    char path[1024];
};


// shared across processes: must be lock-free and of fixed layout
static_assert(std::atomic<int>::is_always_lock_free);
static_assert(sizeof(std::atomic<int>) == sizeof(int));


// content follows, aligned beyond any type alignment
constexpr size_t INFO_SIZE = ((sizeof(Info) + 63) / 64) * 64;


std::string to_hex(key_t key) {
    std::ostringstream str;
    str << "0x" << std::hex << key;
    return str.str();
}


size_t segment_size(size_t file_size) {
    const auto page = static_cast<size_t>(::sysconf(_SC_PAGESIZE));
    return ((INFO_SIZE + file_size + page - 1) / page) * page;
}


/**
 * Removes at exit the segments this process created (not attached, inherited by fork, or since replaced by others);
 * by key, as segment ids are reused once removed
 */
class Unloader {
public:

    static Unloader& instance() {
        static Unloader unloader;
        return unloader;
    }

    void add(key_t key) {
        util::lock_guard<util::recursive_mutex> lock(mutex_);
        keys_.insert(key);
    }

    Unloader(const Unloader&)            = delete;
    Unloader(Unloader&&)                 = delete;
    Unloader& operator=(const Unloader&) = delete;
    Unloader& operator=(Unloader&&)      = delete;

    ~Unloader() {
        for (auto key : keys_) {
            shmid_ds buf{};
            if (auto shmid = Shmget::shmget(key, 0, 0);  // not if removed already
                shmid >= 0 && Shmget::shmctl(shmid, IPC_STAT, &buf) == 0 && buf.shm_cpid == ::getpid()) {
                Shmget::shmctl(shmid, IPC_RMID, nullptr);
            }
        }
    }

private:

    Unloader() = default;

    util::recursive_mutex mutex_;
    std::set<key_t> keys_;
};


}  // namespace


/// An attached segment: read-only except for its creator (loading it); shared by all users (0644)
struct SharedMemory::Segment {
    /// @return nullptr if not permitted (e.g. a segment of another user's, 0600)
    static std::unique_ptr<Segment> attach(int shmid, bool read_only) {
        auto* address = Shmget::shmat(shmid, nullptr, read_only ? SHM_RDONLY : 0);
        if (address == reinterpret_cast<void*>(-1)) {
            // not permitted (another user's), or removed meanwhile (by another process)
            if (errno == EACCES || errno == EINVAL || errno == EIDRM) {
                return nullptr;
            }
            throw FailedSystemCall("shmat");
        }
        return std::unique_ptr<Segment>(new Segment(address));
    }

    Segment(const Segment&)            = delete;
    Segment(Segment&&)                 = delete;
    Segment& operator=(const Segment&) = delete;
    Segment& operator=(Segment&&)      = delete;

    ~Segment() { Shmget::shmdt(address); }

    Info& info() const { return *static_cast<Info*>(address); }
    char* content() const { return static_cast<char*>(address) + INFO_SIZE; }

    /// Of the file (current or previous version), loaded or not; size-checked, as a segment can hold anything
    bool of(const Source& file) const {
        return size >= INFO_SIZE && std::strncmp(info().path, file.path.c_str(), sizeof(Info::path)) == 0;
    }

    /**
     * Being loaded with this file and kind (its creator writes them first), or just created (not yet attached by its
     * creator), by another process still running: not ready, but not stale either
     */
    bool being_loaded(int shmid, const Source& file, int magic) const {
        if (size < INFO_SIZE || info().ready.load(std::memory_order_acquire) != 0) {
            return false;
        }

        const bool loading = info().magic == magic && of(file);
        const bool created = info().magic == 0 && info().path[0] == '\0';
        if (!loading && !created) {
            return false;
        }

        shmid_ds buf{};
        if (Shmget::shmctl(shmid, IPC_STAT, &buf) != 0 || buf.shm_cpid == ::getpid()) {
            return false;
        }

        return ::kill(buf.shm_cpid, 0) == 0 || errno == EPERM;  // running
    }

    /// Completely loaded from the file as it is now
    bool holds(const Source& file) const {
        return of(file) && info().ready.load(std::memory_order_acquire) != 0 && info().version == file.version;
    }

    /// Another kind of content is an error, not a stale segment
    void check_magic(const Source& file, int magic) const {
        if (info().magic != magic) {
            throw CacheError("SharedMemory: '" + file.path + "' loaded as another kind (magic " +
                                 std::to_string(info().magic) + ", expected " + std::to_string(magic) + ")",
                             Here());
        }
    }

    void* const address;
    const size_t size;

private:

    explicit Segment(void* _address) : address(_address), size(Shmget::size(_address)) {}
};


SharedMemory::SharedMemory(const PathName& path, const Options& options) : SharedMemory(path, options, false) {}


SharedMemory::SharedMemory(const PathName& path, const Options& options, bool unload_at_exit) :
    Loader(path), size_(static_cast<size_t>(path.size())), segment_(attach(options, unload_at_exit)) {}


SharedMemory::~SharedMemory() = default;


MemoryUsage SharedMemory::estimate(size_t size) {
    return {0, segment_size(size)};
}


bool SharedMemory::is_loaded(const PathName& path) {
    if (!path.exists()) {
        return false;
    }

    const auto file = Source::of(path);
    if (const auto shmid = Shmget::shmget(file.key, 0, 0); shmid >= 0) {
        const auto segment = Segment::attach(shmid, true);
        return segment && segment->holds(file);
    }
    return false;
}


void SharedMemory::unload(const PathName& path) {
    if (!path.exists()) {
        return;
    }

    // only the segment of this file (any version), not of another file of the same key
    const auto file = Source::of(path);
    if (const auto shmid = Shmget::shmget(file.key, 0, 0); shmid >= 0) {
        if (const auto segment = Segment::attach(shmid, true); segment && segment->of(file)) {
            SYSCALL(Shmget::shmctl(shmid, IPC_RMID, nullptr));
        }
    }
}


const void* SharedMemory::address() const {
    return size_ > 0 ? segment_->content() : nullptr;
}


MemoryUsage SharedMemory::footprint() const {
    return {0, segment_->size};
}


std::unique_ptr<SharedMemory::Segment> SharedMemory::attach(const Options& options, bool unload_at_exit) const {
    // exclusive with creating and loading the file by other threads and processes
    PathLock locker(path());
    util::lock_guard<PathLock> lock(locker);

    const auto file = Source::of(path());
    const auto key  = to_hex(file.key);

    if (file.path.size() >= sizeof(Info::path)) {
        throw CacheError("SharedMemory: path too long '" + file.path + "'", Here());
    }

    if (file.version.size != size_) {
        throw CacheError("SharedMemory: '" + file.path + "' changed while loading", Here());
    }

    auto size = segment_size(size_);
    if (options.huge_pages && options.round_huge_pages) {
        if (const auto page = Shmget::huge_page_size(); page > 0) {
            size = ((size + page - 1) / page) * page;
        }
    }

    int shmid = -1;
    std::unique_ptr<Segment> segment;
    for (;;) {
        // a segment holding the file as it is now is used, one being loaded is waited for, any other is stale (a
        // previous version, an interrupted load, or another file of the same key) and replaced
        if (shmid = Shmget::shmget(file.key, 0, 0); shmid >= 0) {
            auto existing = Segment::attach(shmid, true);
            if (!existing) {
                if (Shmget::shmget(file.key, 0, 0) != shmid) {
                    continue;  // removed meanwhile
                }
                throw FailedSystemCall("SharedMemory: '" + file.path + "', segment (key " + key +
                                       ") cannot be attached (of another user?)");
            }

            if (existing->holds(file)) {
                existing->check_magic(file, options.magic);
                return existing;
            }

            if (existing->being_loaded(shmid, file, options.magic)) {
                existing.reset();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            Log::warning() << "SharedMemory: '" << file.path << "', replacing stale segment (key " << key << ")"
                           << std::endl;

            // processes attached to it are unaffected
            if (Shmget::shmctl(shmid, IPC_RMID, nullptr) != 0 && errno != EINVAL && errno != EIDRM) {
                throw FailedSystemCall("SharedMemory: '" + file.path + "', stale segment (key " + key +
                                       ") cannot be removed (of another user?)");
            }
        }

        // exclusively, so never loaded by two processes (e.g. without a lock file, or of another file of the same key)
        if (shmid = Shmget::create(file.key, size, options.huge_pages); shmid >= 0) {
            if (segment = Segment::attach(shmid, false); segment) {
                break;
            }
            continue;  // removed meanwhile (e.g. by a process without the lock file)
        }

        if (errno != EEXIST) {
            std::ostringstream msg;
            msg << "shmget '" << file.path << "', size " << Bytes(static_cast<double>(size)) << ", key " << key;
            if (const auto max = Shmget::max_size(); max > 0) {
                msg << ", maximum " << Bytes(static_cast<double>(max));
            }
            msg << ", check system limits (Linux: ipcs -l, macOS: ipcs -M)";
            throw FailedSystemCall(msg.str());
        }
    }

    // a new segment is zero-initialised (not ready); what it is loaded with is written first (see being_loaded), and
    // it is removed if loading fails
    auto& info   = segment->info();
    info.magic   = options.magic;
    info.version = file.version;
    std::strncpy(info.path, file.path.c_str(), sizeof(info.path) - 1);

    try {
        Timer timer("SharedMemory: load '" + file.path + "'", Log::debug<LibEcKitCache>());
        read(file.path, segment->content(), size_);
    }
    catch (...) {
        Shmget::shmctl(shmid, IPC_RMID, nullptr);
        throw;
    }

    info.ready.store(1, std::memory_order_release);

    if (unload_at_exit) {
        Unloader::instance().add(file.key);
    }

    return segment;
}


void SharedMemory::print(std::ostream& out) const {
    out << "SharedMemory[path=" << path() << ",size=" << size_ << "]";
}


TempSharedMemory::TempSharedMemory(const PathName& path, const Options& options) : SharedMemory(path, options, true) {}


void TempSharedMemory::print(std::ostream& out) const {
    out << "TempSharedMemory[path=" << path() << ",size=" << size() << "]";
}


}  // namespace eckit::cache::loader
