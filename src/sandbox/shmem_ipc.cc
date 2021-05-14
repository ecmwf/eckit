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
#include <sys/types.h>
#include <unistd.h>

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/runtime/Application.h"


using namespace eckit;

class Test : public Application {
    virtual void run();

public:
    Test(int argc, char** argv) : Application(argc, argv, "HOME") {}
};

const size_t SIZE = 1024;
const char* PATH  = "/eckit-shmem-ipc";

void writer() {
    int fd = SYSCALL(shm_open(PATH, O_RDWR | O_CREAT | O_EXCL, 0644));

    SYSCALL(ftruncate(fd, SIZE));

    void* memptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ASSERT(memptr != MAP_FAILED);

    sem_t* semptr = sem_open(PATH, O_CREAT, 0644, 0);
    ASSERT(semptr != SEM_FAILED);

    ::memcpy(memptr, "hello\0", 6);
    SYSCALL(sem_post(semptr));

    sleep(12);
    SYSCALL(munmap(memptr, SIZE));
    SYSCALL(close(fd));
    SYSCALL(sem_close(semptr));
    SYSCALL(shm_unlink(PATH));
    SYSCALL(sem_unlink(PATH));
}

void reader() {
    int fd = SYSCALL(shm_open(PATH, O_RDWR, 0644));

    // SYSCALL(ftruncate(fd, SIZE));

    void* memptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ASSERT(memptr != MAP_FAILED);

    sem_t* semptr = sem_open(PATH, O_CREAT, 0644, 0);
    ASSERT(semptr != SEM_FAILED);

    SYSCALL(sem_wait(semptr));
    std::cout << (char*)memptr << std::endl;

    SYSCALL(munmap(memptr, SIZE));
    SYSCALL(close(fd));
    SYSCALL(sem_close(semptr));
}


void Test::run() {
    if (Resource<bool>("--reader", false)) {
        reader();
    }
    else {
        writer();
    }
}

//=============================================================

int main(int argc, char** argv) {
    Test app(argc, argv);
    app.start();
    return 0;
}
