// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <iosfwd>

namespace eckit::net {

//----------------------------------------------------------------------------------------------------------------------

class Socket {
public:  // methods

    virtual ~Socket() = default;

    virtual bool isConnected() const = 0;

    virtual long write(const void* buf, long length) const = 0;
    virtual long read(void* buf, long length) const = 0;

    virtual void close() = 0;
    virtual void closeOutput() = 0;
    virtual void closeInput() = 0;

    /// The underlying file descriptor
    virtual int socket() = 0;

protected:  // methods

    Socket()                         = default;
    Socket(const Socket&)            = default;
    Socket& operator=(const Socket&) = default;

    virtual void print(std::ostream& s) const = 0;

    friend std::ostream& operator<<(std::ostream& s, const Socket& socket) {
        socket.print(s);
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::net

