// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/os/SharedInt.h"

#include <sys/sem.h>

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"


namespace eckit {


SharedInt::SharedInt(const PathName& path, int count) : Semaphore(path, 2 * count) {}

SharedInt::~SharedInt() {}

void SharedInt::use(int n) {
    Semaphore::lower(2 * n);
}

void SharedInt::use(int n, short v) {
    Semaphore::lower(2 * n, v);
}

void SharedInt::release(int n) {
    Semaphore::raise(2 * n);
}

void SharedInt::release(int n, short v) {
    Semaphore::raise(2 * n, v);
}

void SharedInt::newLimit(short val, unsigned short n) {
    int v;
    while ((v = semctl(semaphore_, 2 * n + 1, GETVAL, 0)) != val) {
        if (v < 0) {
            throw FailedSystemCall("semctl GETVAL");
        }
        short delta         = val - v;
        using U             = unsigned short int;
        struct sembuf set[] = {{
                                   U(2 * n),
                                   delta,
                                   0,
                               },
                               {
                                   U(2 * n + 1),
                                   delta,
                                   0,
                               }};
        SYSCALL(semop(semaphore_, set, NUMBER(set)));
    }
}

int SharedInt::limit(int n) const {
    return Semaphore::get(2 * n + 1);
}

int SharedInt::free(int n) const {
    return Semaphore::get(2 * n);
}

}  // namespace eckit
