// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <unistd.h>

#include <atomic>
#include <string>
#include <thread>
#include <vector>

#include "eckit/testing/Test.h"
#include "eckit/thread/ConcurrencyLimiter.h"

using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("An unlimited limiter always has a slot") {
    ConcurrencyLimiter limiter("unlimited", 0);
    std::string reason;
    for (int i = 0; i < 100; ++i) {
        EXPECT(limiter.tryAcquire(reason));
    }
    EXPECT_EQUAL(limiter.active(), 100);
}

CASE("tryAcquire fails when all slots are in use, and succeeds once one is released") {
    ConcurrencyLimiter limiter("limited", 2);
    std::string reason;
    EXPECT(limiter.tryAcquire(reason));
    EXPECT(limiter.tryAcquire(reason));
    EXPECT(reason.empty());

    EXPECT(!limiter.tryAcquire(reason));
    EXPECT(!reason.empty());
    EXPECT_EQUAL(limiter.active(), 2);

    limiter.release();
    EXPECT_EQUAL(limiter.active(), 1);
    EXPECT(limiter.tryAcquire(reason));
    EXPECT_EQUAL(limiter.active(), 2);
}

CASE("AutoConcurrencySlot releases on scope exit") {
    ConcurrencyLimiter limiter("auto", 1);
    std::string reason;
    {
        EXPECT(limiter.tryAcquire(reason));
        AutoConcurrencySlot slot(limiter);
        EXPECT_EQUAL(limiter.active(), 1);
        EXPECT(!limiter.tryAcquire(reason));
    }
    EXPECT_EQUAL(limiter.active(), 0);
}

CASE("Concurrent activities never exceed the limit") {
    const long max = 3;
    ConcurrencyLimiter limiter("concurrent", max);

    std::atomic<long> running{0};
    std::atomic<long> peak{0};
    std::atomic<long> refused{0};

    std::vector<std::thread> threads;
    for (int i = 0; i < 16; ++i) {
        threads.emplace_back([&] {
            for (int j = 0; j < 50; ++j) {
                std::string reason;
                if (!limiter.tryAcquire(reason)) {
                    ++refused;
                    continue;
                }
                AutoConcurrencySlot slot(limiter);
                long n = ++running;
                long p = peak;
                while (n > p && !peak.compare_exchange_weak(p, n)) {
                }
                ::usleep(100);
                --running;
            }
        });
    }
    for (auto& t : threads) {
        t.join();
    }

    EXPECT(peak <= max);
    EXPECT(refused > 0);
    EXPECT_EQUAL(limiter.active(), 0);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
