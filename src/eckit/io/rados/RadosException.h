// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Nicolau Manubens
/// @author Metin Çakırcalı
/// @date March 2024

#pragma once

#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/CodeLocation.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// A failure reported by the Ceph cluster or its client library, or stored data which is not as expected.
/// @note: errors with a meaning of their own are thrown as the eckit exception for it instead:
///   NotFound, AlreadyExists, PermissionDenied and OutOfStorage. See call_rados().
class RadosException : public eckit::Exception {
public:

    /// @param code the (negative) error number returned by librados, or 0 if the failure is not one of librados
    explicit RadosException(const std::string& msg, const eckit::CodeLocation& loc = {}, int code = 0);

    int code() const { return code_; }

private:

    int code_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
