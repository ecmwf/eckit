// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   SyscallFault.h
///
/// Lets a test make the next close, fsync, fdatasync or fclose issued through eckit's wrappers fail with a chosen
/// errno (eckit::closeFd, eckit::closeFile, eckit::fsync, eckit::fdatasync). Without an active SyscallFault the
/// wrappers behave as the plain system calls.

#pragma once

#include <atomic>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

enum class Syscall {
    Close,
    Fsync,
    Fdatasync,
    Fclose,
};

/// Makes calls of one kind fail while in scope.
///
/// A failing close or fclose still releases the descriptor or stream, as the system calls do; fsync and fdatasync
/// are not called. Only one SyscallFault may be active at a time. It may be used from any thread, but must outlive
/// every call that can reach it, so create it before the code under test starts and destroy it after it ends.
class SyscallFault {
public:

    static constexpr int anyDescriptor = -1;

    /// Fails the next @p count calls of kind @p call on descriptor @p fd (any descriptor with anyDescriptor) with
    /// errno @p error.
    SyscallFault(Syscall call, int fd, int error, int count = 1);

    SyscallFault(const SyscallFault&)            = delete;
    SyscallFault& operator=(const SyscallFault&) = delete;
    SyscallFault(SyscallFault&&)                 = delete;
    SyscallFault& operator=(SyscallFault&&)      = delete;

    ~SyscallFault();

    /// Number of failures not yet injected.
    int remaining() const { return remaining_.load(); }

    /// The errno to fail a call of kind @p call on @p fd with, or 0 to let it run. Used by the wrappers.
    static int injected(Syscall call, int fd) noexcept;

private:

    int consume(Syscall call, int fd) noexcept;

    Syscall call_;
    int fd_;
    int error_;
    std::atomic<int> remaining_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
