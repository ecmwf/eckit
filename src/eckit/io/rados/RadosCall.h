// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   RadosCall.h
/// @author Metin Çakırcalı
/// @date   November 2026
///
/// What the implementation of the RADOS classes needs from librados: checked calls and owners of
/// the librados handles. Code which only uses the RADOS classes does not need this header.

#pragma once

#include <rados/librados.h>

#include <limits>
#include <memory>
#include <vector>

#include "eckit/log/CodeLocation.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// Checks the value returned by a librados call.
/// @returns the value, if it is not an error
/// @throws NotFound (not logged), AlreadyExists, PermissionDenied or OutOfStorage for the errors they
///   describe, and RadosException, which carries the error number, for any other
int call_rados(int code, const char* call, const CodeLocation& loc);

/// Same, but an ENOENT error is reported by returning false instead of throwing.
bool call_rados_found(int code, const char* call, const CodeLocation& loc);

#define RADOS_CALL(a) eckit::call_rados(a, #a, Here())
#define RADOS_CALL_FOUND(a) eckit::call_rados_found(a, #a, Here())

/// librados rejects a read of more bytes than this
constexpr long radosMaxReadSize = std::numeric_limits<int>::max();

//----------------------------------------------------------------------------------------------------------------------

/// Completion of an asynchronous operation
class RadosAIO {
public:

    RadosAIO();
    ~RadosAIO();

    RadosAIO(const RadosAIO&)            = delete;
    RadosAIO& operator=(const RadosAIO&) = delete;

    /// Waits for the operation and checks its result.
    /// @note: must only be called once the operation has been submitted, or it never returns
    void waitForComplete() const;

    /// Waits for every completion and empties the list, also if some of them failed.
    /// Rethrows the first failure once all completions have been waited for.
    static void waitForAll(std::vector<std::unique_ptr<RadosAIO>>& comps);

    rados_completion_t comp_{nullptr};
};

//----------------------------------------------------------------------------------------------------------------------

class RadosWriteOp {
public:

    RadosWriteOp();
    ~RadosWriteOp();

    RadosWriteOp(const RadosWriteOp&)            = delete;
    RadosWriteOp& operator=(const RadosWriteOp&) = delete;

    rados_write_op_t op_;
};

//----------------------------------------------------------------------------------------------------------------------

class RadosReadOp {
public:

    RadosReadOp();
    ~RadosReadOp();

    RadosReadOp(const RadosReadOp&)            = delete;
    RadosReadOp& operator=(const RadosReadOp&) = delete;

    rados_read_op_t op_;
};

//----------------------------------------------------------------------------------------------------------------------

/// Result of a read of keys or values of a key-value
class RadosIter {
public:

    RadosIter() = default;
    ~RadosIter();

    RadosIter(const RadosIter&)            = delete;
    RadosIter& operator=(const RadosIter&) = delete;

    rados_omap_iter_t it_{nullptr};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
