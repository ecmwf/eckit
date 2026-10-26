// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <fcntl.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <string>
#include <thread>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Close.h"
#include "eckit/io/FDataSync.h"
#include "eckit/io/SyscallFault.h"
#include "eckit/testing/Test.h"

namespace eckit::test {

namespace {

class TempFile {
public:

    TempFile() : path_{"syscallfault_" + std::to_string(::getpid()) + "_" + std::to_string(counter_++)} {}
    ~TempFile() { ::unlink(path_.c_str()); }

    int open() const { return ::open(path_.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644); }
    FILE* fopen() const { return std::fopen(path_.c_str(), "w"); }

private:

    static inline int counter_ = 0;
    std::string path_;
};

bool isOpen(int fd) {
    return ::fcntl(fd, F_GETFD) != -1;
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("Without a fault the wrappers behave as the system calls") {
    TempFile file;
    const int fd = file.open();
    EXPECT(fd >= 0);
    EXPECT_EQUAL(eckit::fsync(fd), 0);
    EXPECT_EQUAL(eckit::fdatasync(fd), 0);
    EXPECT_EQUAL(closeFd(fd), 0);
    EXPECT(!isOpen(fd));

    FILE* f = file.fopen();
    EXPECT(f != nullptr);
    EXPECT_EQUAL(closeFile(f), 0);
}

CASE("closeFd fails with the chosen errno and still releases the descriptor") {
    TempFile file;
    const int fd = file.open();

    SyscallFault fault(Syscall::Close, fd, EIO);
    EXPECT_EQUAL(closeFd(fd), -1);
    EXPECT_EQUAL(errno, EIO);
    EXPECT(!isOpen(fd));
    EXPECT_EQUAL(fault.remaining(), 0);
}

CASE("closeFile fails with the chosen errno and still releases the stream") {
    TempFile file;
    FILE* f      = file.fopen();
    const int fd = ::fileno(f);

    SyscallFault fault(Syscall::Fclose, fd, ENOSPC);
    EXPECT_EQUAL(closeFile(f), EOF);
    EXPECT_EQUAL(errno, ENOSPC);
    EXPECT(!isOpen(fd));
}

CASE("fsync and fdatasync fail with the chosen errno") {
    TempFile file;
    const int fd = file.open();
    {
        SyscallFault fault(Syscall::Fsync, fd, EIO);
        EXPECT_EQUAL(eckit::fsync(fd), -1);
        EXPECT_EQUAL(errno, EIO);
        EXPECT_EQUAL(eckit::fsync(fd), 0);
    }
    {
        SyscallFault fault(Syscall::Fdatasync, SyscallFault::anyDescriptor, EIO);
        EXPECT_EQUAL(eckit::fdatasync(fd), -1);
        EXPECT_EQUAL(errno, EIO);
    }
    EXPECT_EQUAL(closeFd(fd), 0);
}

CASE("A fault affects only its kind of call and its descriptor, and only while in scope") {
    TempFile a;
    TempFile b;
    const int fdA = a.open();
    const int fdB = b.open();
    {
        SyscallFault fault(Syscall::Fsync, fdA, EIO, 2);
        EXPECT_EQUAL(eckit::fdatasync(fdA), 0);
        EXPECT_EQUAL(eckit::fsync(fdB), 0);
        EXPECT_EQUAL(eckit::fsync(fdA), -1);
        EXPECT_EQUAL(fault.remaining(), 1);
    }
    EXPECT_EQUAL(eckit::fsync(fdA), 0);
    EXPECT_EQUAL(closeFd(fdA), 0);
    EXPECT_EQUAL(closeFd(fdB), 0);
}

CASE("Only one fault may be active at a time") {
    SyscallFault fault(Syscall::Close, SyscallFault::anyDescriptor, EIO);
    auto second = [] { SyscallFault other(Syscall::Fsync, SyscallFault::anyDescriptor, EIO); };
    EXPECT_THROWS_AS(second(), AssertionFailed);
    EXPECT_EQUAL(closeFd(-1), -1);
    EXPECT_EQUAL(errno, EIO);
}

CASE("A fault is seen from another thread") {
    TempFile file;
    const int fd = file.open();
    int result   = 0;
    int error    = 0;
    {
        SyscallFault fault(Syscall::Fsync, fd, EIO);
        std::thread other([&] {
            result = eckit::fsync(fd);
            error  = errno;
        });
        other.join();
    }
    EXPECT_EQUAL(result, -1);
    EXPECT_EQUAL(error, EIO);
    EXPECT_EQUAL(closeFd(fd), 0);
}

CASE("FailedSystemCall keeps the errno of the failed call") {
    errno = 0;
    try {
        SYSCALL(closeFd(-1));
    }
    catch (const FailedSystemCall& e) {
        EXPECT_EQUAL(e.errorNumber(), EBADF);
        return;
    }
    EXPECT(false);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
