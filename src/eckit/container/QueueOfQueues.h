/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @date   September 2026

#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <exception>
#include <mutex>

#include "eckit/container/Queue.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// A bounded queue of bounded Queues, which pops elements in the order the sub-queues were pushed.
///
/// Each producer claims a sub-queue with push(), fills it at its own pace, and close()s it when
/// done. The consumer pops elements from the front sub-queue; once that one is closed and drained
/// it is discarded and popping continues with the next. So producers may run concurrently and out
/// of step, but the consumer still sees their output strictly in push() order.
///
/// Both dimensions are bounded, and both apply back-pressure: at most `maxQueues` sub-queues may be
/// outstanding (push() blocks beyond that, limiting how far ahead the producers may run) and each
/// holds at most `maxQueueSize` elements (Queue::push() blocks beyond that, limiting what a single
/// producer may buffer).
///
/// @note Single consumer. pop() may not be called concurrently from more than one thread: a
///   sub-queue is destroyed as soon as it has been drained, and only the sole consumer can know
///   that no other consumer is still inside it. Any number of producers is fine, and in-order
///   output is the point of the structure, so a single consumer is the natural use anyway.
///
/// @note A sub-queue that is never close()d stalls the consumer permanently once it reaches the
///   front. A producer that fails must therefore either close() its sub-queue - leaving the
///   elements already pushed into it to be consumed - or interrupt() the whole QueueOfQueues. For
///   the same reason a producer should close() its sub-queue before push()ing another: holding an
///   unclosed one while blocked in push() stalls the consumer, and thereby itself.
template <typename ELEM>
class QueueOfQueues {

public:  // types

    using SubQueue = Queue<ELEM>;

public:  // methods

    QueueOfQueues(size_t maxQueues, size_t maxQueueSize = 100) :
        maxQueues_(maxQueues), maxQueueSize_(maxQueueSize), interrupt_{nullptr}, closed_(false) {
        ASSERT(maxQueues > 0);
        ASSERT(maxQueueSize > 0);
    }

    QueueOfQueues(const QueueOfQueues&)            = delete;
    QueueOfQueues& operator=(const QueueOfQueues&) = delete;

    // n.b. cannot move object with std::condition_variable
    QueueOfQueues(QueueOfQueues&& rhs)            = delete;
    QueueOfQueues& operator=(QueueOfQueues&& rhs) = delete;

    size_t maxQueues() const { return maxQueues_; }

    size_t maxQueueSize() const { return maxQueueSize_; }

    /// The number of sub-queues currently outstanding, not the number of elements in them.
    size_t size() {
        std::unique_lock<std::mutex> locker(mutex_);
        return queues_.size();
    }

    /// True if no sub-queues are currently outstanding.
    bool empty() {
        std::unique_lock<std::mutex> locker(mutex_);
        return queues_.empty();
    }

    /// No further sub-queues will be pushed. Those already pushed remain usable, and are consumed
    /// as normal - it is the producers' business to close() them.
    void close() {
        std::unique_lock<std::mutex> locker(mutex_);
        closed_ = true;
        cv_.notify_all();
    }

    bool closed() {
        std::unique_lock<std::mutex> locker(mutex_);
        return closed_ || interrupt_;
    }

    bool checkInterrupt() {
        if (interrupt_) {
            std::rethrow_exception(interrupt_);
        }
        return true;
    }

    /// Propagates the exception to everybody working on this structure: to the consumer in pop(),
    /// to any producer waiting in push(), and - through every outstanding sub-queue - to the
    /// producers filling those. push() rethrows it thereafter, so no sub-queue can escape unaware
    /// of it.
    void interrupt(std::exception_ptr expn) {
        std::unique_lock<std::mutex> locker(mutex_);
        interrupt_ = expn;
        for (auto& queue : queues_) {
            queue.interrupt(expn);
        }
        cv_.notify_all();
    }

    /// Appends a new, empty sub-queue and returns it for the caller to fill and then close().
    ///
    /// Blocks while maxQueues() sub-queues are already outstanding.
    ///
    /// @note The reference is valid only until the consumer discards the sub-queue, which it does
    ///       as soon as it is both closed and drained.
    SubQueue& push() {
        std::unique_lock<std::mutex> locker(mutex_);
        while (checkInterrupt() && queues_.size() >= maxQueues_) {
            cv_.wait(locker);
        }
        ASSERT(!closed_);
        SubQueue& queue = queues_.emplace_back(maxQueueSize_);
        cv_.notify_all();
        return queue;
    }

    /// Pops the next element from the front sub-queue, discarding sub-queues that are closed and
    /// drained until one yields an element.
    ///
    /// @returns the number of elements left in the sub-queue it came from, or -1 if this
    ///          QueueOfQueues is closed and no sub-queues remain.
    long pop(ELEM& e) {

        ConsumerGuard guard(consuming_);

        while (true) {

            SubQueue* front = nullptr;
            {
                std::unique_lock<std::mutex> locker(mutex_);
                while (checkInterrupt() && queues_.empty()) {
                    if (closed_) {
                        return -1;
                    }
                    cv_.wait(locker);
                }
                front = &queues_.front();
            }

            // n.b. outside the lock: this blocks until the front sub-queue has something to give,
            // and both push() and interrupt() must remain callable meanwhile. It throws if the
            // sub-queue has been interrupted, and returns -1 once closed and drained. Nothing can
            // discard the sub-queue under us, as only the (sole) consumer ever discards.
            //
            // If we wish to make this work with multiple consumers, we need to own the sub-queue
            // a shared pointer.

            long remaining = front->pop(e);
            if (remaining >= 0) {
                return remaining;
            }

            std::unique_lock<std::mutex> locker(mutex_);
            checkInterrupt();
            ASSERT(!queues_.empty() && &queues_.front() == front);
            queues_.pop_front();
            cv_.notify_all();
        }
    }

private:  // types

    /// Enforces the single-consumer contract. Without it a second consumer would be left holding a
    /// sub-queue that the first one had already discarded and destroyed - a use-after-free that no
    /// sanitiser would reliably catch, since the deque keeps owning the storage.
    class ConsumerGuard {
    public:

        explicit ConsumerGuard(std::atomic<bool>& consuming) : consuming_(consuming) {
            ASSERT_MSG(!consuming_.exchange(true), "QueueOfQueues::pop() is single-consumer only");
        }
        ~ConsumerGuard() { consuming_ = false; }

        ConsumerGuard(const ConsumerGuard&)            = delete;
        ConsumerGuard& operator=(const ConsumerGuard&) = delete;

    private:

        std::atomic<bool>& consuming_;
    };

private:  // members

    std::deque<SubQueue> queues_;
    std::mutex mutex_;
    std::condition_variable cv_;
    size_t maxQueues_;
    size_t maxQueueSize_;
    std::exception_ptr interrupt_;
    bool closed_;
    std::atomic<bool> consuming_{false};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
