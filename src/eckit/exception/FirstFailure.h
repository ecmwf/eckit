// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   FirstFailure.h
/// @date   Oct 2026

#pragma once

#include <exception>
#include <ostream>
#include <utility>

#include "eckit/log/Log.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// Runs a sequence of steps that must all be attempted, then reports the first failure.
///
/// \code{.cpp}
/// FirstFailure failure;
/// failure.run([&] { flush(); });
/// failure.run([&] { release(); });  // attempted even if flush() threw
/// failure.rethrow();                // throws what flush() threw, if it did
/// \endcode
///
/// Later failures are dropped, as a caller can only handle one, but they are logged.
///
/// Not thread safe: use it from one thread, and synchronise externally to hand it to another.
class FirstFailure {
public:

    FirstFailure() = default;

    // a copy would hold the failure too, and report it a second time
    FirstFailure(const FirstFailure&)            = delete;
    FirstFailure& operator=(const FirstFailure&) = delete;
    FirstFailure(FirstFailure&&)                 = delete;
    FirstFailure& operator=(FirstFailure&&)      = delete;

    ~FirstFailure() = default;

    /// Runs `step`, keeping its failure if it is the first.
    template <typename S>
    void run(S&& step) noexcept {
        try {
            std::forward<S>(step)();
        }
        catch (...) {
            if (!first_) {
                first_ = std::current_exception();
            }
            else {
                logDropped(std::current_exception());
            }
        }
    }

    /// Throws the first failure, once. Does nothing if every step succeeded.
    void rethrow() {
        if (first_) {
            std::rethrow_exception(std::exchange(first_, nullptr));
        }
    }

private:

    static void logDropped(const std::exception_ptr& failure) noexcept {
        try {
            try {
                std::rethrow_exception(failure);
            }
            catch (const std::exception& error) {
                Log::error() << "** Failure dropped, as an earlier one is reported: " << error.what() << std::endl;
            }
            catch (...) {
                Log::error() << "** Failure dropped, as an earlier one is reported" << std::endl;
            }
        }
        catch (...) {
            // a failing log must not turn a dropped failure into a crash
        }
    }

    std::exception_ptr first_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
