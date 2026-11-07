// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/memory/Shmget.h"

#include <sys/ipc.h>

#include <map>

#include "eckit/thread/AutoLock.h"
#include "eckit/thread/StaticMutex.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

namespace {

// Track the segments attached by this process and each's size by addr
struct Attached {
    static Attached& instance() {
        // never destroyed, as segments can be detached at exit (by static objects)
        // value-initialised (parentheses, no user-provided constructor), as StaticMutex relies on zeroed memory
        static auto* attached = new Attached();
        return *attached;
    }

    StaticMutex mutex;
    std::map<const void*, size_t> sizes;
    size_t size = 0;
};

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

int Shmget::shmget(key_t key, size_t size, int shmflg) {
    return ::shmget(key, size, shmflg);
}

void* Shmget::shmat(int shmid, const void* shmaddr, int shmflg) {
    void* addr = ::shmat(shmid, shmaddr, shmflg);
    if (addr == reinterpret_cast<void*>(-1)) {
        return addr;
    }

    if (addr != nullptr) {
        // the segment's size, as a segment can be attached without knowing it (e.g. got with size 0)
        shmid_ds buf{};
        const auto segment_size = shmctl(shmid, IPC_STAT, &buf) == 0 ? static_cast<size_t>(buf.shm_segsz) : 0;

        auto& attached = Attached::instance();
        AutoLock<StaticMutex> lock(attached.mutex);

        attached.sizes[addr] = segment_size;
        attached.size += segment_size;
    }

    return addr;
}

int Shmget::shmdt(const void* shmaddr) {
    const int code = ::shmdt(shmaddr);
    if (code == 0) {
        auto& attached = Attached::instance();
        AutoLock<StaticMutex> lock(attached.mutex);

        if (auto it = attached.sizes.find(shmaddr); it != attached.sizes.end()) {
            attached.size -= it->second;
            attached.sizes.erase(it);
        }
    }

    return code;
}

int Shmget::shmctl(int shmid, int cmd, shmid_ds* buf) {
    return ::shmctl(shmid, cmd, buf);
}

void Shmget::info(size_t& count, size_t& size) {
    auto& attached = Attached::instance();
    AutoLock<StaticMutex> lock(attached.mutex);

    count = attached.sizes.size();
    size  = attached.size;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
