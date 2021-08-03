/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <errno.h>
#include <sys/sem.h>
#include <cstdio>

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/os/Semaphore.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

struct sembuf _lock[] = {
    {0, 0, SEM_UNDO}, /* test */
    {0, 1, SEM_UNDO}, /* lock */
};

struct sembuf _try_lock[] = {
    {0, 0, SEM_UNDO | IPC_NOWAIT}, /* test */
    {0, 1, SEM_UNDO},              /* lock */
};

struct sembuf _unlock[] = {
    {0, -1, SEM_UNDO}, /* ulck */
};

Semaphore::Semaphore(const PathName& name, int count) :
    semaphore_(-1), count_(count), level_(0) {
    key_t key = ftok(name.localPath(), 1);

    if (key == key_t(-1) && errno == ENOENT) {
        name.touch();
        key = ftok(name.localPath(), 1);
    }

    if (key == key_t(-1))
        throw FailedSystemCall(std::string("ftok(") + name + std::string(")"));

    /// @note cannot use Log::debug() of SYSCALL here, because Log may not yet be initialized
    // std::cout << "Creating semaphore path=" << name << ", count=" << count << ", key=" << hex <<
    // key << dec << std::endl;
    if ((semaphore_ = semget(key, count_, 0666 | IPC_CREAT)) < 0)
        perror("semget failed"), throw FailedSystemCall("semget");
}

Semaphore::~Semaphore() {
    ASSERT(level_ == 0);
}

void Semaphore::lock(void) {
    mutex_.lock();
    if (++level_ == 1)
        while (semop(semaphore_, _lock, NUMBER(_lock)) < 0) {
            if (errno != EINTR) {
                --level_;
                mutex_.unlock();
                throw FailedSystemCall("semop lock");
            }
        }
}

bool Semaphore::tryLock(void) {
    if (!mutex_.tryLock())
        return false;
    if (++level_ == 1)
        if (semop(semaphore_, _try_lock, NUMBER(_try_lock)) < 0) {
            --level_;
            mutex_.unlock();
            if (errno == EAGAIN)
                return false;
            else
                throw FailedSystemCall("semop try_lock");
        }
    return true;
}

void Semaphore::unlock(void) {
    ASSERT(level_ > 0);

    if (--level_ == 0)
        while (semop(semaphore_, _unlock, NUMBER(_unlock)) < 0) {
            if (errno != EINTR) {
                ++level_;
                throw FailedSystemCall("semop unlock");
            }
        }

    mutex_.unlock();
}

bool Semaphore::test(unsigned short n) {
    struct sembuf test = {n, 0, IPC_NOWAIT};

    if (semop(semaphore_, &test, 1) == 0)
        return false;  // Free

    if (errno == EAGAIN)
        return true;  // in use

    throw FailedSystemCall("semop test");
}

pid_t Semaphore::getpid() const {
    int val;
    SYSCALL(val = semctl(semaphore_, 0, GETPID));
    return val;
}

void Semaphore::set(int val, int n) {
    SYSCALL(semctl(semaphore_, n, SETVAL, val));
}


int Semaphore::get(int n) const {
    int val;
    SYSCALL(val = semctl(semaphore_, n, GETVAL, 0));
    return val;
}

void Semaphore::raise(unsigned short n) {
    struct sembuf op = {n, 1, SEM_UNDO};
    SYSCALL(semop(semaphore_, &op, 1));
}

void Semaphore::raise(unsigned short n, short v) {
    struct sembuf op = {n, v, SEM_UNDO};
    SYSCALL(semop(semaphore_, &op, 1));
}

void Semaphore::lower(unsigned short n) {
    struct sembuf op = {n, -1, SEM_UNDO};
    SYSCALL(semop(semaphore_, &op, 1));
}

void Semaphore::lower(unsigned short n, short v) {
    short d          = -v;
    struct sembuf op = {n, d, SEM_UNDO};
    SYSCALL(semop(semaphore_, &op, 1));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
