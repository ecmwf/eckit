// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/util/Shmget.h"

#include <sys/ipc.h>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/os/Stat.h"


namespace eckit::geo::util {


key_t Shmget::calculate_key(const PathName& path) {
    const auto real = path.realName();

    Stat::Struct s;
    SYSCALL(Stat::stat(real.localPath(), &s));

    // a rebuilt file (inode, modification time) maps to a different segment; proj_id 0 is undefined for ftok
    const auto key = ::ftok(real.localPath(), static_cast<int>(s.st_mtime % 255) + 1);
    if (key == static_cast<key_t>(-1)) {
        throw FailedSystemCall("ftok(" + real.asString() + ")");
    }

    return key;
}


int Shmget::shmget(key_t key, size_t size) {
    constexpr int shmflg = IPC_CREAT | 0600;

#ifdef SHM_HUGETLB
    // huge pages: fewer page-table entries and page faults for the processes attaching to the segment (the kernel
    // rounds the size up to the huge page size)
    const int shmid = shmget(key, size, LibEcKitGeo::searchSharedMemoryHugePages() ? (shmflg | SHM_HUGETLB) : shmflg);
#else
    const int shmid = shmget(key, size, shmflg);
#endif

#ifdef SHM_PAGESIZE
    if (shmid >= 0) {
        // back the shared memory with 64K pages, if possible (failing is not an error)
        shmid_ds buf{};
        buf.shm_pagesize = 64 * 1024;
        shmctl(shmid, SHM_PAGESIZE, &buf);
    }
#endif

    return shmid;
}


int Shmget::shmget(key_t key, size_t size, int shmflg) {
    int shmid = eckit::Shmget::shmget(key, size, shmflg);
#ifdef SHM_HUGETLB
    if (shmid < 0 && (shmflg & SHM_HUGETLB) != 0) {
        // no huge pages reserved on this node, fall back to regular pages
        shmid = eckit::Shmget::shmget(key, size, shmflg & ~SHM_HUGETLB);
    }
#endif
    return shmid;
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


}  // namespace eckit::geo::util
