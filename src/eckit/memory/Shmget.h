// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date Dec 2017

#ifndef eckit_memory_Shmget_h
#define eckit_memory_Shmget_h

#include <sys/shm.h>  // for key_t

#include <cstddef>


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// System V shared memory, accounting for segments attached by this process (reported by eckit::system::MemoryInfo)
class Shmget {

public:  // methods

    /// Get or create a segment, as ::shmget
    /// @return the segment id, or -1 on failure (see errno)
    static int shmget(key_t key, size_t size, int shmflg);

    /// Attach a segment, accounted for until detached
    /// @return the segment address, or (void*)-1 on failure, not nullptr (see errno)
    static void* shmat(int shmid, const void* shmaddr, int shmflg);

    /// Detach a segment, no longer accounted for once detached
    /// @return 0, or -1 on failure (see errno)
    static int shmdt(const void* shmaddr);

    /// Control a segment (e.g. IPC_STAT, IPC_RMID), as ::shmctl; removing a segment doesn't affect attached processes
    /// @return 0 for most commands, >= 0 for some (e.g. IPC_INFO), or -1 on failure (see errno)
    static int shmctl(int shmid, int cmd, shmid_ds* buf);

    static void info(size_t& count, size_t& size);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
