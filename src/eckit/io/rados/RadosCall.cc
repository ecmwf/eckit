// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/rados/RadosCall.h"

#include <rados/librados.h>

#include <cerrno>
#include <cstring>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/io/rados/RadosException.h"
#include "eckit/log/CodeLocation.h"
#include "eckit/log/Log.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

int call_rados(int code, const char* call, const CodeLocation& loc) {

    if (code >= 0) {
        return code;
    }

    std::ostringstream oss;
    oss << "RADOS call " << call << " failed [" << code << "] (" << std::strerror(-code) << ")";
    const std::string msg = oss.str();

    LOG_DEBUG_LIB(LibEcKit) << msg << std::endl;

    switch (-code) {
        case ENOENT:
            /// @note: an ordinary outcome which callers handle, so it is not logged as an error
            throw NotFound(msg, loc, true);
        case EEXIST:
            throw AlreadyExists(msg, loc);
        case EPERM:
        case EACCES:
            throw PermissionDenied(msg, loc);
        case ENOSPC:
        case EDQUOT:
            throw OutOfStorage(msg, loc);
        default:
            throw RadosException(msg, loc, code);
    }
}

bool call_rados_found(int code, const char* call, const CodeLocation& loc) {

    if (code == -ENOENT) {
        LOG_DEBUG_LIB(LibEcKit) << "RADOS call " << call << ": not found" << std::endl;
        return false;
    }

    call_rados(code, call, loc);

    return true;
}

//----------------------------------------------------------------------------------------------------------------------

RadosAIO::RadosAIO() {
    RADOS_CALL(rados_aio_create_completion(nullptr, nullptr, nullptr, &comp_));
}

RadosAIO::~RadosAIO() {
    rados_aio_release(comp_);
}

void RadosAIO::waitForComplete() const {
    RADOS_CALL(rados_aio_wait_for_complete(comp_));
    RADOS_CALL(rados_aio_get_return_value(comp_));
}

void RadosAIO::waitForAll(std::vector<std::unique_ptr<RadosAIO>>& comps) {

    /// @note: what was pending is no longer so, whether the wait succeeds or not
    std::vector<std::unique_ptr<RadosAIO>> waited;
    waited.swap(comps);

    forAllThenRethrowFirst(waited, [](const std::unique_ptr<RadosAIO>& comp) { comp->waitForComplete(); });
}

//----------------------------------------------------------------------------------------------------------------------

RadosWriteOp::RadosWriteOp() : op_(rados_create_write_op()) {}

RadosWriteOp::~RadosWriteOp() {
    rados_release_write_op(op_);
}

//----------------------------------------------------------------------------------------------------------------------

RadosReadOp::RadosReadOp() : op_(rados_create_read_op()) {}

RadosReadOp::~RadosReadOp() {
    rados_release_read_op(op_);
}

//----------------------------------------------------------------------------------------------------------------------

RadosIter::~RadosIter() {
    if (it_) {
        rados_omap_get_end(it_);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
