/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_net_TCPSocket_h
#define eckit_net_TCPSocket_h

#include <netinet/in.h>

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/net/SocketOptions.h"
#include "eckit/utils/Hash.h"

namespace eckit::net {

/// @note this class calls sets a handler to ignore SIGPIPE

class TCPSocket {

public:  // types

    class UnknownHost : public Exception {
    public:

        explicit UnknownHost(const std::string&);
    };

public:  // methods

    TCPSocket();

    // From an existing TCPSocket (see TCPServer::accept)

    /// @warning
    /// **** NOTE: copying gives ownership of the socket to new object
    /// **** Beware of 'slicing', i.e copying subclasses.

    TCPSocket(net::TCPSocket&);

    virtual ~TCPSocket();

    // See note on copy constructor

    TCPSocket& operator=(net::TCPSocket&);

    long write(const void* buf, long length) const;

    /// Read from a TCP socket
    ///
    /// \param buf The buffer to read into
    /// \param length The maximum number of bytes to read
    ///
    /// **Configuration flags**
    /// \arg **useSelectOnTCPSocket** (*bool*): use select for improved resilience
    ///   on flaky connections
    /// \arg **socketSelectTimeout** (*long*): timeout in seconds for the select
    ///   (only if **useSelectOnTCPSocket** is enabled)
    long read(void* buf, long length) const;

    long rawRead(void*, long);  // Non-blocking version

    bool isConnected() const { return socket_ != -1; }

    bool stillConnected() const;

    // close sockets
    virtual void close();

    // peer name

    in_addr remoteAddr() const;
    const std::string& remoteHost() const;
    int remotePort() const;

    in_addr localAddr() const;
    const std::string& localHost() const;
    int localPort() const;

    void bufferSize(int n) {
        receiveBufferSize(n);
        sendBufferSize(n);
    }

    void receiveBufferSize(int n) { receiveBufferSize_ = n; }

    void sendBufferSize(int n) { sendBufferSize_ = n; }

    virtual int socket();

    void closeOutput();
    void closeInput();

    void debug(bool on);

public:  // class methods

    static std::string addrToHost(in_addr);
    static in_addr hostToAddr(const std::string&);
    static std::string hostName(const std::string& h, bool full = false);

    /// @note uses sigaction to ignore SIGPIPE
    static void register_ignore_sigpipe();

protected:  // members

    int socket_;              // i/o socket
    int localPort_;           // effective port
    int remotePort_;          // remote port
    std::string remoteHost_;  // remote host
    in_addr remoteAddr_;      // remote ip adress
    std::string localHost_;   // local host
    in_addr localAddr_;       // local ip adress

    int receiveBufferSize_ = 0;
    int sendBufferSize_    = 0;

    // Debug
    struct {
        bool on{false};
        mutable bool newline{true};
        mutable char mode{0};
    } debug_;

protected:  // methods

    int createSocket(int port, const SocketOptions& options);

    virtual void print(std::ostream& s) const;

private:  // methods

    /// @pre socket must be made
    virtual void bind();
    virtual std::string bindingAddress() const;

    friend std::ostream& operator<<(std::ostream& s, const TCPSocket& socket) {
        socket.print(s);
        return s;
    }
};

std::ostream& operator<<(std::ostream&, in_addr);

}  // namespace eckit::net

#endif
