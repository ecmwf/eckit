// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <unistd.h>

#include <chrono>
#include <csignal>
#include <string>
#include <thread>
#include <vector>

#include "eckit/net/NetService.h"
#include "eckit/net/NetUser.h"
#include "eckit/net/TCPClient.h"
#include "eckit/net/TCPStream.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/testing/Test.h"
#include "eckit/thread/ConcurrencyLimiter.h"
#include "eckit/thread/ThreadControler.h"

using namespace eckit;
using namespace eckit::net;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

using Clock = std::chrono::steady_clock;

static double since(Clock::time_point start) {
    return std::chrono::duration<double>(Clock::now() - start).count();
}

static int testPort(int offset) {
    return 20000 + (::getpid() % 2000) * 10 + offset;
}

// Reads a number of seconds, replies with its pid, sleeps, then replies "done".
// When refused, replies -1 instead of a pid.

class SleepUser : public NetUser {
public:

    explicit SleepUser(TCPSocket& socket) : NetUser(socket) {}

private:

    void serve(Stream& s, std::istream&, std::ostream&) override {
        int secs;
        s >> secs;
        s << int(::getpid());
        ::sleep(secs);
        s << std::string("done");
    }

    void refuse(Stream& s, std::istream&, std::ostream&, const std::string&) override {
        int secs;
        s >> secs;
        s << int(-1);
    }
};

class SleepService : public NetService {
public:

    SleepService(int port, ConcurrencyLimiter& limiter, bool process) :
        NetService(port, limiter, false), process_(process) {}

private:

    bool process_;

    NetUser* newUser(TCPSocket& socket) const override { return new SleepUser(socket); }
    std::string name() const override { return "limitertest"; }
    bool preferToRunAsProcess() const override { return process_; }
};

static void startService(int port, ConcurrencyLimiter& limiter, bool process) {
    ThreadControler t(new SleepService(port, limiter, process));
    t.start();
    ::usleep(200000);
}

struct Result {
    int pid = 0;
    std::string reply;
    double elapsed = 0;
};

static Result request(int port, int secs) {
    Result r;
    Clock::time_point start = Clock::now();
    TCPClient client;
    TCPSocket& socket = client.connect("localhost", port);
    InstantTCPStream s(socket);
    s << secs;
    s >> r.pid;
    if (r.pid > 0) {
        s >> r.reply;
    }
    r.elapsed = since(start);
    return r;
}

static std::vector<Result> parallelRequests(int port, const std::vector<int>& secs) {
    std::vector<Result> results(secs.size());
    std::vector<std::thread> threads;
    for (size_t i = 0; i < secs.size(); ++i) {
        threads.emplace_back([&, i] { results[i] = request(port, secs[i]); });
        ::usleep(100000);  // Fix the arrival order
    }
    for (auto& t : threads) {
        t.join();
    }
    return results;
}

//----------------------------------------------------------------------------------------------------------------------

// Slots of child processes are released asynchronously, once the process has terminated
static void waitUntilIdle(const ConcurrencyLimiter& limiter) {
    Clock::time_point start = Clock::now();
    while (limiter.active() != 0 && since(start) < 5) {
        ::usleep(20000);
    }
}

static void checkLimits(int port, bool process) {
    // The service is never stopped, so the limiter must outlive the test
    static ConcurrencyLimiter threadLimiter("thread", 2);
    static ConcurrencyLimiter processLimiter("process", 2);
    ConcurrencyLimiter& limiter = process ? processLimiter : threadLimiter;
    startService(port, limiter, process);

    // Two are served concurrently, the third is refused immediately
    std::vector<Result> r = parallelRequests(port, {2, 2, 2});

    EXPECT(r[0].pid > 0);
    EXPECT(r[1].pid > 0);
    EXPECT_EQUAL(r[0].reply, "done");
    EXPECT_EQUAL(r[1].reply, "done");
    EXPECT(r[0].elapsed < 4);
    EXPECT(r[1].elapsed < 4);

    EXPECT_EQUAL(r[2].pid, -1);
    EXPECT(r[2].elapsed < 0.5);

    if (process) {
        EXPECT(r[0].pid != ::getpid());
        EXPECT(r[0].pid != r[1].pid);
    }
    else {
        EXPECT_EQUAL(r[0].pid, ::getpid());
    }

    // Slots are released when the connections end
    waitUntilIdle(limiter);
    EXPECT_EQUAL(limiter.active(), 0);

    // ... after which connections are served again
    r = parallelRequests(port, {0, 0});
    EXPECT(r[0].pid > 0);
    EXPECT(r[1].pid > 0);
    EXPECT_EQUAL(r[1].reply, "done");
}

CASE("NetService limits concurrent connections served in threads") {
    checkLimits(testPort(0), false);
}

CASE("NetService limits concurrent connections served in processes") {
    checkLimits(testPort(1), true);
}

CASE("The slot of a killed child process is released") {
    int port   = testPort(2);
    static ConcurrencyLimiter limiter("kill", 1);
    startService(port, limiter, true);

    TCPClient client;
    TCPSocket& socket = client.connect("localhost", port);
    InstantTCPStream s(socket);
    s << int(600);
    int pid;
    s >> pid;
    EXPECT(pid > 0 && pid != ::getpid());
    EXPECT_EQUAL(limiter.active(), 1);

    ::kill(pid, SIGKILL);

    waitUntilIdle(limiter);
    EXPECT_EQUAL(limiter.active(), 0);

    Result r = request(port, 0);
    EXPECT(r.pid > 0);
    EXPECT_EQUAL(r.reply, "done");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
