// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <sys/types.h>

#include "eckit/net/UnixSocket.h"

namespace eckit::net {

//----------------------------------------------------------------------------------------------------------------------

class UnixSocketServer : public UnixSocket {
public:  // methods

    explicit UnixSocketServer(const LocalPathName& path, int backlog = 128);
    ~UnixSocketServer() override;

    void close() override;

    std::unique_ptr<Socket> acceptSocket(const std::string& message = "Waiting for connection",
                                         int timeout                = 0) override;

protected:  // methods

    void print(std::ostream& s) const override;

private:  // members

    pid_t owner_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::net
