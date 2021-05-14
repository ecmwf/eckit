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
const char* FULL  = "/eckit-shmem-ipc-full";
const char* EMPTY = "/eckit-shmem-ipc-empty";


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

    sem_t* full = sem_open(FULL, O_CREAT, 0644, 0);
    ASSERT(full != SEM_FAILED);
    sem_t* empty = sem_open(EMPTY, O_CREAT, 0644, 1);
    ASSERT(empty != SEM_FAILED);

    for (size_t i = 0; i < 10; i++) {
        SYSCALL(sem_wait(empty));
        ::memcpy(memptr, "hello\0", 6);
        SYSCALL(sem_post(full));
    }


    SYSCALL(munmap(memptr, SIZE));
    SYSCALL(close(fd));
    SYSCALL(sem_close(full));
    SYSCALL(sem_close(empty));

    // SYSCALL(shm_unlink(PATH));
    // SYSCALL(sem_unlink(FULL));
    // SYSCALL(sem_unlink(EMPTY));
}

void reader() {
    int fd = SYSCALL2(shm_open(PATH, O_RDWR, 0644), PATH);

    // SYSCALL(ftruncate(fd, SIZE));

    void* memptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ASSERT(memptr != MAP_FAILED);

    sem_t* full = sem_open(FULL, O_CREAT, 0644, 0);
    ASSERT(full != SEM_FAILED);
    sem_t* empty = sem_open(EMPTY, O_CREAT, 0644, 1);
    ASSERT(empty != SEM_FAILED);

    for (;;) {
        SYSCALL(sem_wait(full));
        std::cout << (char*)memptr << std::endl;
        SYSCALL(sem_post(empty));
    }

    SYSCALL(munmap(memptr, SIZE));
    SYSCALL(close(fd));
    SYSCALL(sem_close(empty));
    SYSCALL(sem_close(full));
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
