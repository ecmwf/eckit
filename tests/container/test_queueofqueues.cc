/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/container/QueueOfQueues.h"
#include "eckit/testing/Test.h"

#include <atomic>
#include <chrono>
#include <stdexcept>
#include <thread>
#include <vector>

using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

/// Drains until the QueueOfQueues reports itself closed and empty.
std::vector<int> drain(QueueOfQueues<int>& qq) {
    std::vector<int> out;
    int value = 0;
    while (qq.pop(value) != -1) {
        out.push_back(value);
    }
    return out;
}

//----------------------------------------------------------------------------------------------------------------------

CASE("Elements come out in sub-queue order, whatever order the sub-queues are filled in") {

    QueueOfQueues<int> qq(4, 8);

    auto& q1 = qq.push();
    auto& q2 = qq.push();
    auto& q3 = qq.push();

    // Filled back to front, and interleaved
    q3.push(30);
    q2.push(20);
    q3.push(31);
    q1.push(10);
    q2.push(21);
    q1.push(11);

    q1.close();
    q2.close();
    q3.close();
    qq.close();

    EXPECT(drain(qq) == std::vector<int>({10, 11, 20, 21, 30, 31}));
}

CASE("Empty sub-queues are skipped over") {

    QueueOfQueues<int> qq(4, 4);

    auto& q1 = qq.push();
    auto& q2 = qq.push();
    auto& q3 = qq.push();

    q2.push(20);

    q1.close();  // never had anything pushed into it
    q2.close();
    q3.close();  // ditto
    qq.close();

    EXPECT(drain(qq) == std::vector<int>({20}));
}

CASE("An exhausted QueueOfQueues reports -1 only once closed") {

    QueueOfQueues<int> qq(2, 2);

    auto& q1 = qq.push();
    q1.push(7);
    q1.close();

    int value = 0;
    EXPECT_EQUAL(qq.pop(value), 0);  // no elements left in q1
    EXPECT_EQUAL(value, 7);

    // q1 is closed and drained, but until the QueueOfQueues itself is closed pop() must keep
    // waiting for the next sub-queue rather than reporting the end.
    constexpr long PENDING = -2;
    std::atomic<long> result{PENDING};
    std::thread consumer([&] {
        int v  = 0;
        result = qq.pop(v);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQUAL(result.load(), PENDING);

    qq.close();
    consumer.join();
    EXPECT_EQUAL(result.load(), -1);

    // ... and the exhausted q1 has been discarded along the way
    EXPECT(qq.empty());
}

CASE("The number of outstanding sub-queues is bounded") {

    QueueOfQueues<int> qq(2, 4);

    auto& q1 = qq.push();
    auto& q2 = qq.push();
    EXPECT_EQUAL(qq.size(), size_t(2));

    std::atomic<bool> pushed{false};
    std::thread producer([&] {
        auto& q3 = qq.push();  // blocks: two sub-queues are already outstanding
        pushed   = true;
        q3.push(30);
        q3.close();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT(!pushed.load());

    q1.push(10);
    q1.close();
    q2.close();

    // Popping everything q1 holds also discards q1 and the empty q2, making room for q3
    std::vector<int> out;
    int value = 0;
    for (int i = 0; i < 2; ++i) {
        EXPECT(qq.pop(value) >= 0);
        out.push_back(value);
    }

    producer.join();
    EXPECT(pushed.load());
    EXPECT(out == std::vector<int>({10, 30}));

    qq.close();
    EXPECT(drain(qq).empty());
}

CASE("A consumer blocked on the front sub-queue is interrupted") {

    QueueOfQueues<int> qq(2, 2);
    auto& q1 = qq.push();  // pushed but never filled, so pop() blocks inside it

    std::atomic<bool> caught{false};
    std::thread consumer([&] {
        try {
            int v = 0;
            qq.pop(v);
        }
        catch (const std::runtime_error&) {
            caught = true;
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT(!caught.load());

    qq.interrupt(std::make_exception_ptr(std::runtime_error("boom")));
    consumer.join();
    EXPECT(caught.load());
}

CASE("A consumer waiting for its first sub-queue is interrupted") {

    QueueOfQueues<int> qq(2, 2);

    std::atomic<bool> caught{false};
    std::thread consumer([&] {
        try {
            int v = 0;
            qq.pop(v);
        }
        catch (const std::runtime_error&) {
            caught = true;
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    qq.interrupt(std::make_exception_ptr(std::runtime_error("boom")));
    consumer.join();
    EXPECT(caught.load());
}

CASE("The interrupt reaches producers filling their sub-queues") {

    QueueOfQueues<int> qq(2, 1);  // depth 1, so the second push into a sub-queue blocks

    auto& q1 = qq.push();

    std::atomic<bool> caught{false};
    std::thread producer([&] {
        try {
            q1.push(1);
            q1.push(2);  // blocks: nobody is consuming
        }
        catch (const std::runtime_error&) {
            caught = true;
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT(!caught.load());

    qq.interrupt(std::make_exception_ptr(std::runtime_error("boom")));
    producer.join();
    EXPECT(caught.load());

    // The sub-queue reports the interrupt to whoever still holds it, too
    EXPECT(q1.closed());
    EXPECT_THROWS_AS(q1.checkInterrupt(), std::runtime_error);
}

CASE("The interrupt reaches a producer waiting for a free sub-queue") {

    QueueOfQueues<int> qq(1, 2);
    auto& q1 = qq.push();

    std::atomic<bool> caught{false};
    std::thread producer([&] {
        try {
            qq.push();  // blocks: only one sub-queue is permitted
        }
        catch (const std::runtime_error&) {
            caught = true;
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT(!caught.load());

    qq.interrupt(std::make_exception_ptr(std::runtime_error("boom")));
    producer.join();
    EXPECT(caught.load());
}

CASE("Sub-queues pushed after an interrupt would carry it, but push() rethrows first") {

    QueueOfQueues<int> qq(2, 2);
    qq.interrupt(std::make_exception_ptr(std::runtime_error("boom")));

    EXPECT_THROWS_AS(qq.push(), std::runtime_error);
    EXPECT_THROWS_AS(qq.checkInterrupt(), std::runtime_error);
    EXPECT(qq.closed());

    int value = 0;
    EXPECT_THROWS_AS(qq.pop(value), std::runtime_error);
}

CASE("A second concurrent consumer is rejected rather than left with a discarded sub-queue") {

    QueueOfQueues<int> qq(2, 2);
    auto& q1 = qq.push();  // never filled, so the first consumer parks inside it

    std::atomic<bool> parked{false};
    std::atomic<bool> rejected{false};

    std::thread first([&] {
        try {
            int v  = 0;
            parked = true;
            qq.pop(v);
        }
        catch (const std::runtime_error&) {
        }
    });

    while (!parked.load()) {
        std::this_thread::yield();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    try {
        int v = 0;
        qq.pop(v);
    }
    catch (const eckit::AssertionFailed&) {
        rejected = true;
    }
    EXPECT(rejected.load());

    qq.interrupt(std::make_exception_ptr(std::runtime_error("boom")));
    first.join();

    // The rejected consumer must not have cleared the flag on its way out
    EXPECT_THROWS_AS(q1.checkInterrupt(), std::runtime_error);
}

CASE("Concurrent producers still yield their output in push() order") {

    constexpr size_t NPRODUCERS = 8;
    constexpr size_t NITEMS     = 40;
    constexpr size_t PERITEM    = 25;

    QueueOfQueues<int> qq(NPRODUCERS, 4);  // small sub-queues, so producers must interleave

    std::atomic<size_t> next{0};
    std::vector<std::thread> producers;

    for (size_t p = 0; p < NPRODUCERS; ++p) {
        producers.emplace_back([&] {
            while (true) {
                Queue<int>* q = nullptr;
                size_t item   = 0;
                {
                    // Claiming the item and its sub-queue together is what fixes the output order
                    static std::mutex claim;
                    std::unique_lock<std::mutex> locker(claim);
                    item = next++;
                    if (item >= NITEMS) {
                        return;
                    }
                    q = &qq.push();
                }
                for (size_t k = 0; k < PERITEM; ++k) {
                    q->push(static_cast<int>(item * 1000 + k));
                }
                q->close();
            }
        });
    }

    std::vector<int> out;
    int value = 0;
    while (out.size() < NITEMS * PERITEM) {
        EXPECT(qq.pop(value) >= 0);
        out.push_back(value);
    }

    for (auto& p : producers) {
        p.join();
    }

    std::vector<int> expected;
    for (size_t item = 0; item < NITEMS; ++item) {
        for (size_t k = 0; k < PERITEM; ++k) {
            expected.push_back(static_cast<int>(item * 1000 + k));
        }
    }
    EXPECT(out == expected);

    qq.close();
    EXPECT(drain(qq).empty());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
