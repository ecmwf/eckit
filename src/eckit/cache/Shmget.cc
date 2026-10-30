// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/Shmget.h"

#include <sys/ipc.h>

#include <cerrno>
#include <fstream>
#include <map>
#include <sstream>
#include <string>

#include "eckit/cache/mutex.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/os/Stat.h"


namespace eckit::cache {


namespace {


/// Segments attached by this process, size by address
struct Attached {
    static Attached& instance() {
        // never destroyed, as segments can be detached at exit (by static objects)
        static auto* attached = new Attached;
        return *attached;
    }

    util::recursive_mutex mutex;
    std::map<const void*, size_t> sizes;
    size_t size = 0;
};


}  // namespace


key_t calculate_shmget_key_from_path(const PathName& path) {
    Stat::Struct s;
    SYSCALL(Stat::stat(path.localPath(), &s));
    return calculate_shmget_key_from_path(path, s.st_mtime);
}


key_t calculate_shmget_key_from_path(const PathName& path, time_t modified) {
    // mtime in proj_id so a rebuilt file usually maps to another key; proj_id 0 is undefined for ftok
    const auto key = ::ftok(path.localPath(), static_cast<int>(modified % 255) + 1);
    if (key == static_cast<key_t>(-1)) {
        throw FailedSystemCall("ftok(" + path.asString() + ")");
    }

    return key;
}


int Shmget::create(key_t key, size_t size, bool huge_pages) {
    constexpr int shmflg = IPC_CREAT | IPC_EXCL | 0644;

#ifdef SHM_HUGETLB
    // some systems require a multiple of the huge page size (see Loader::Options::round_huge_pages)
    const int shmid = shmget(key, size, huge_pages ? (shmflg | SHM_HUGETLB) : shmflg);
#else
    if (huge_pages) {
        Log::warning() << "Shmget: huge pages requested, but not supported on this system, using regular pages"
                       << std::endl;
    }
    const int shmid = shmget(key, size, shmflg);
#endif

#ifdef SHM_PAGESIZE
    if (shmid >= 0) {
        // 64K pages if possible (failing is not an error)
        shmid_ds buf{};
        buf.shm_pagesize = 64 * 1024;
        shmctl(shmid, SHM_PAGESIZE, &buf);
    }
#endif

    return shmid;
}


int Shmget::shmget(key_t key, size_t size, int shmflg) {
    int shmid = ::shmget(key, size, shmflg);
#ifdef SHM_HUGETLB
    // fall back only if huge pages are the problem (not EEXIST or EACCES)
    if (shmid < 0 && (shmflg & SHM_HUGETLB) != 0 && (errno == EINVAL || errno == ENOMEM || errno == EPERM)) {
        Log::warning() << "Shmget: huge pages requested, but not available (none reserved, or the size not a multiple "
                          "of the huge page size), using regular pages"
                       << std::endl;
        shmid = ::shmget(key, size, shmflg & ~SHM_HUGETLB);
    }
#endif
    return shmid;
}


void* Shmget::shmat(int shmid, const void* shmaddr, int shmflg) {
    auto* address = ::shmat(shmid, shmaddr, shmflg);
    if (address == reinterpret_cast<void*>(-1)) {
        return address;
    }

    // segment size, as created
    shmid_ds buf{};
    const auto size = shmctl(shmid, IPC_STAT, &buf) == 0 ? static_cast<size_t>(buf.shm_segsz) : 0;

    auto& attached = Attached::instance();
    util::lock_guard<util::recursive_mutex> lock(attached.mutex);

    attached.sizes[address] = size;
    attached.size += size;

    return address;
}


size_t Shmget::size(const void* shmaddr) {
    auto& attached = Attached::instance();
    util::lock_guard<util::recursive_mutex> lock(attached.mutex);

    auto it = attached.sizes.find(shmaddr);
    return it != attached.sizes.end() ? it->second : 0;
}


int Shmget::shmdt(const void* shmaddr) {
    const int code = ::shmdt(shmaddr);
    if (code != 0) {
        return code;
    }

    auto& attached = Attached::instance();
    util::lock_guard<util::recursive_mutex> lock(attached.mutex);

    if (auto it = attached.sizes.find(shmaddr); it != attached.sizes.end()) {
        attached.size -= it->second;
        attached.sizes.erase(it);
    }

    return code;
}


int Shmget::shmctl(int shmid, int cmd, shmid_ds* buf) {
    return ::shmctl(shmid, cmd, buf);
}


size_t Shmget::max_size() {
#ifdef IPC_INFO
    shminfo info{};
    if (shmctl(0, IPC_INFO, reinterpret_cast<shmid_ds*>(&info)) >= 0) {
        return static_cast<size_t>(info.shmmax);
    }
#endif
    return 0;
}


size_t Shmget::huge_page_size() {
    // as reported by Linux, e.g. "Hugepagesize:       2048 kB"
    std::ifstream in("/proc/meminfo");
    for (std::string line; std::getline(in, line);) {
        if (line.rfind("Hugepagesize:", 0) == 0) {
            std::istringstream value(line.substr(13));
            size_t kb = 0;
            value >> kb;
            return kb * 1024;
        }
    }
    return 0;
}


Shmget::Info Shmget::info() {
    auto& attached = Attached::instance();
    util::lock_guard<util::recursive_mutex> lock(attached.mutex);

    return {attached.sizes.size(), attached.size};
}


}  // namespace eckit::cache
