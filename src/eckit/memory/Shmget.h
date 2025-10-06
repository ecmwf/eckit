/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
