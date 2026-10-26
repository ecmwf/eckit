// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/testing/Test.h"

#if defined(__linux__)

#include <fcntl.h>
#include <unistd.h>

#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

#include "eckit/testing/FaultInjection.h"

namespace eckit::test {

using namespace eckit::testing;

namespace {

std::string tempPath(const std::string& name) {
    return "fault_injection_" + name + "_" + std::to_string(::getpid());
}

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexceptions"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wterminate"
#endif

struct ThrowingDestructor {
    ~ThrowingDestructor() noexcept(true) {
        std::cerr << "throwing from a noexcept destructor" << std::endl;
        throw std::runtime_error("destructor");
    }
};

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("openDescriptorCount counts a newly opened descriptor") {
    const int before = openDescriptorCount();
    const int fd     = ::open("/dev/null", O_RDONLY);
    EXPECT(fd >= 0);
    EXPECT_EQUAL(openDescriptorCount(), before + 1);
    ::close(fd);
    EXPECT_EQUAL(openDescriptorCount(), before);
}

CASE("closeDescriptorOf makes the owner's next close fail with EBADF") {
    const std::string path = tempPath("closeof");
    const int fd           = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    EXPECT(fd >= 0);

    EXPECT_EQUAL(closeDescriptorOf(path), fd);
    EXPECT_EQUAL(::close(fd), -1);
    EXPECT_EQUAL(errno, EBADF);

    EXPECT_THROWS_AS(closeDescriptorOf(path), std::runtime_error);
    ::unlink(path.c_str());
}

CASE("DescriptorSentinel detects a second close of the same number") {
    const int fd = ::open("/dev/null", O_RDONLY);
    EXPECT(fd >= 0);
    ::close(fd);

    DescriptorSentinel sentinel(fd);
    EXPECT_EQUAL(sentinel.fd(), fd);
    EXPECT(sentinel.stillOpen());

    ::close(fd);
    EXPECT(!sentinel.stillOpen());
}

CASE("DescriptorSentinel refuses a descriptor that is still open") {
    const int fd = ::open("/dev/null", O_RDONLY);
    EXPECT(fd >= 0);
    EXPECT_THROWS_AS(DescriptorSentinel{fd}, std::runtime_error);
    ::close(fd);
}

CASE("devFullAvailable: a buffered write to /dev/full fails when flushed") {
    if (!devFullAvailable()) {
        std::cout << "/dev/full not available, skipped" << std::endl;
        return;
    }
    FILE* f = std::fopen("/dev/full", "w");
    EXPECT(f != nullptr);
    std::fputs("data", f);
    EXPECT_EQUAL(std::fflush(f), EOF);
    EXPECT_EQUAL(errno, ENOSPC);
    std::fclose(f);
}

CASE("runInChild reports each way a child can end, with its stderr") {
    auto completed = runInChild([] { std::cerr << "hello" << std::endl; });
    EXPECT_EQUAL(completed.outcome, ChildOutcome::Completed);
    EXPECT_EQUAL(completed.output, std::string("hello\n"));

    EXPECT_EQUAL(runInChild([] { throw std::runtime_error("thrown"); }).outcome, ChildOutcome::Threw);
    EXPECT_EQUAL(runInChild([] { throw 42; }).outcome, ChildOutcome::Threw);
    EXPECT_EQUAL(runInChild([] { std::abort(); }).outcome, ChildOutcome::Aborted);
    EXPECT_EQUAL(runInChild([] { ::_exit(3); }).outcome, ChildOutcome::Other);
}

CASE("runInChild reports Terminated for a noexcept destructor that throws") {
    auto result = runInChild([] { ThrowingDestructor d; });
    EXPECT_EQUAL(result.outcome, ChildOutcome::Terminated);
    EXPECT(result.output.find("throwing from a noexcept destructor") != std::string::npos);
}

CASE("runInChild refuses to fork a process with another thread") {
    EXPECT_EQUAL(threadCount(), 1);
    bool refused = false;
    {
        std::thread other([&] {});
        try {
            runInChild([] {});
        }
        catch (const std::logic_error&) {
            refused = true;
        }
        other.join();
    }
    EXPECT(refused);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

#endif  // __linux__

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
