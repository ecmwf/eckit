// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "eckit/net/UnixSocket.h"

namespace eckit::net {

//----------------------------------------------------------------------------------------------------------------------

class UnixSocketClient : public UnixSocket {
public:  // methods

    UnixSocketClient() = default;

    /// Connect to the server listening on path.
    /// throws FailedSystemCall if no server is listening
    UnixSocket& connect(const LocalPathName& path);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::net
