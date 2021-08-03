/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>

#include <string>

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/runtime/Application.h"


using namespace eckit;

class Test : public Application {
    virtual void run();

public:
    Test(int argc, char** argv) :
        Application(argc, argv, "HOME") {}
};

const size_t SIZE = 1024;
const char* PATH  = "/eckit-shmem-ipc";
const char* FULL  = "/eckit-shmem-ipc-full";
const char* EMPTY = "/eckit-shmem-ipc-empty";


class NamedSemaphore {
public:
    NamedSemaphore(const std::string& name, int value, bool create = true, bool unlink = false, int mode = 0664);
    ~NamedSemaphore();

    void wait();
    void post();

private:
    std::string name_;
    bool unlink_;
    sem_t* semaphore_ = SEM_FAILED;
};

NamedSemaphore::NamedSemaphore(const std::string& name, int value, bool create, bool unlink, int mode) :
    name_(name), unlink_(unlink) {
    semaphore_ = sem_open(name_.c_str(), create ? O_CREAT : 0, mode, value);
    if (semaphore_ == SEM_FAILED) {
        throw FailedSystemCall("sem_open(" + name_ + ")");
    }
}

NamedSemaphore::~NamedSemaphore() {
    if (semaphore_ != SEM_FAILED) {
        SYSCALL(::sem_close(semaphore_));
    }
    if (unlink_) {
        SYSCALL(::sem_unlink(name_.c_str()));
    }
}

void NamedSemaphore::wait() {
    SYSCALL(::sem_wait(semaphore_));
}
void NamedSemaphore::post() {
    SYSCALL(::sem_post(semaphore_));
}

void writer() {

    // shm_unlink(PATH);


    int fd = SYSCALL(shm_open(PATH, O_RDWR | O_CREAT, 0644));

    struct stat s;
    SYSCALL(fstat(fd, &s));
    std::cout << "SIZE " << s.st_size << std::endl;

    if (s.st_size == 0) {
        SYSCALL(ftruncate(fd, SIZE));
    }

    void* memptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ASSERT(memptr != MAP_FAILED);

    NamedSemaphore full(FULL, 0);
    NamedSemaphore empty(EMPTY, 1);


    for (size_t i = 0; i < 10; i++) {
        empty.wait();
        ::memcpy(memptr, "hello\0", 6);
        full.post();
    }


    SYSCALL(munmap(memptr, SIZE));
    SYSCALL(close(fd));

    // SYSCALL(shm_unlink(PATH));
    // SYSCALL(sem_unlink(FULL));
    // SYSCALL(sem_unlink(EMPTY));
}

void reader() {
    int fd = SYSCALL2(shm_open(PATH, O_RDWR, 0644), PATH);

    // SYSCALL(ftruncate(fd, SIZE));

    void* memptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ASSERT(memptr != MAP_FAILED);

    NamedSemaphore full(FULL, 0);
    NamedSemaphore empty(EMPTY, 1);

    for (;;) {
        full.wait();
        std::cout << (char*)memptr << std::endl;
        empty.post();
    }

    SYSCALL(munmap(memptr, SIZE));
    SYSCALL(close(fd));
}


void Test::run() {
    if (Resource<bool>("--reader", false)) {
        reader();
    }
    else {
        writer();
    }
}

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    Test app(argc, argv);
    app.start();
    return 0;
}
