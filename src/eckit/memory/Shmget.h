// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date Dec 2017

#ifndef eckit_memory_Shmget_h
#define eckit_memory_Shmget_h

#include <sys/shm.h>  // for key_t

#include <iosfwd>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Shmget {

public:  // methods

    static int shmget(key_t key, size_t size, int shmflg);
    static void* shmat(int shmid, const void* shmaddr, int shmflg);
    static int shmdt(const void* shmaddr);

    static void info(size_t& count, size_t& size);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
