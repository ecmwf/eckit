// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <string>
#include <utility>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/FileFlock.h"
#include "eckit/testing/Test.h"

using namespace eckit;

namespace {

//----------------------------------------------------------------------------------------------------------------------

LocalPathName lockPath() {
    return LocalPathName("eckit_test_fileflock." + std::to_string(::getpid()) + ".lock");
}

CASE("An exclusive lock excludes all other locks, even in the same process") {
    FileFlock a(lockPath());
    FileFlock b(lockPath());

    EXPECT(a.tryLockExclusive());
    EXPECT(!b.tryLockExclusive());
    EXPECT(!b.tryLockShared());

    a.unlock();
    EXPECT(b.tryLockExclusive());
}

CASE("Shared locks coexist, and exclude an exclusive lock") {
    FileFlock a(lockPath());
    FileFlock b(lockPath());
    FileFlock c(lockPath());

    EXPECT(a.tryLockShared());
    EXPECT(b.tryLockShared());
    EXPECT(!c.tryLockExclusive());
}

CASE("An exclusive lock converts to a shared one") {
    FileFlock a(lockPath());
    FileFlock b(lockPath());

    EXPECT(a.tryLockExclusive());
    EXPECT(!b.tryLockShared());

    EXPECT(a.tryLockShared());
    EXPECT(b.tryLockShared());
}

CASE("Closing another descriptor of the file does not release a lock") {
    FileFlock a(lockPath());
    EXPECT(a.tryLockExclusive());

    { FileFlock other(lockPath()); }
    ::close(::open(lockPath().localPath(), O_RDONLY));

    FileFlock b(lockPath());
    EXPECT(!b.tryLockShared());
}

CASE("Destruction releases the lock") {
    {
        FileFlock a(lockPath());
        EXPECT(a.tryLockExclusive());
    }
    FileFlock b(lockPath());
    EXPECT(b.tryLockExclusive());
}

CASE("A forked child shares the lock, which is held until the child also closes it") {
    int pipefd[2];
    EXPECT(::pipe(pipefd) == 0);

    FileFlock a(lockPath());
    EXPECT(a.tryLockExclusive());

    pid_t pid = ::fork();
    if (pid == 0) {
        char c;
        ::close(pipefd[1]);
        ::read(pipefd[0], &c, 1);  // Hold the inherited lock until the parent closes the pipe
        ::_exit(0);
    }
    ::close(pipefd[0]);

    a.close();
    FileFlock b(lockPath());
    EXPECT(!b.tryLockExclusive());

    ::close(pipefd[1]);
    int status = 0;
    EXPECT(::waitpid(pid, &status, 0) == pid);
    EXPECT(b.tryLockExclusive());
}

CASE("The file is opened on the first lock, and without create a missing file throws then") {
    LocalPathName missing(lockPath() + ".missing");

    FileFlock a(missing, false);
    EXPECT_THROWS_AS(a.tryLockShared(), FailedSystemCall);

    FileFlock b(missing);
    EXPECT(!missing.exists());
    EXPECT(b.tryLockShared());
    EXPECT(missing.exists());
    ::unlink(missing.localPath());
}

CASE("Moving transfers the lock") {
    FileFlock a(lockPath());
    EXPECT(a.tryLockExclusive());

    FileFlock b;
    b = std::move(a);
    a.close();

    FileFlock c(lockPath());
    EXPECT(!c.tryLockShared());

    b.close();
    EXPECT(c.tryLockShared());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace

int main(int argc, char** argv) {
    int result = eckit::testing::run_tests(argc, argv);
    ::unlink(lockPath().localPath());
    return result;
}
