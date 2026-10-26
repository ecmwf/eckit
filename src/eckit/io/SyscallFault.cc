// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/SyscallFault.h"

#include "eckit/exception/Exceptions.h"

namespace eckit {

namespace {

std::atomic<SyscallFault*> active_{nullptr};

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

SyscallFault::SyscallFault(Syscall call, int fd, int error, int count) :
    call_{call}, fd_{fd}, error_{error}, remaining_{count} {
    ASSERT(error_ != 0);
    ASSERT(count > 0);

    SyscallFault* expected = nullptr;
    const bool installed   = active_.compare_exchange_strong(expected, this);
    ASSERT_MSG(installed, "Only one SyscallFault may be active at a time");
}

SyscallFault::~SyscallFault() {
    SyscallFault* expected = this;
    active_.compare_exchange_strong(expected, nullptr);
}

int SyscallFault::consume(Syscall call, int fd) noexcept {
    if (call != call_ || (fd_ != anyDescriptor && fd != fd_)) {
        return 0;
    }
    int left = remaining_.load();
    while (left > 0) {
        if (remaining_.compare_exchange_weak(left, left - 1)) {
            return error_;
        }
    }
    return 0;
}

int SyscallFault::injected(Syscall call, int fd) noexcept {
    SyscallFault* fault = active_.load(std::memory_order_acquire);
    return fault == nullptr ? 0 : fault->consume(call, fd);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
