/*
 * (C) Copyright 2021- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_net_MultiSocket_h
#define eckit_net_MultiSocket_h

#include <netinet/in.h>
#include <string>
#include <vector>

#include "eckit/io/Select.h"

namespace eckit::net {

class TCPServer;
class TCPSocket;

class MultiSocket {
public:

    MultiSocket(size_t streams, size_t messageSize);  // Client
    MultiSocket(int port);                            // Server
    ~MultiSocket();

    MultiSocket& accept();
    MultiSocket& connect(const std::string& host, int port, int retries = 5, int timeout = 0);

    MultiSocket(MultiSocket&);  // Take ownership

    long write(const void* buf, long length);
    long read(void* buf, long length);

    // close sockets
    void close();

    // peer name

    in_addr remoteAddr() const;
    const std::string& remoteHost() const;
    int remotePort() const;

    in_addr localAddr() const;
    const std::string& localHost() const;
    int localPort() const;

    void bufferSize(int n) { bufferSize_ = n; }

    void closeOutput();
    void closeInput();

    void debug(bool on);

private:  // methods

    MultiSocket(const MultiSocket&);
    MultiSocket& operator=(const MultiSocket&);

    void print(std::ostream& s) const;
    void cleanup();

    // ---

    Select select_;
    TCPServer* accept_ = nullptr;
    std::vector<TCPSocket*> sockets_;

    size_t streams_ = 0;

    int readSocket_   = 0;
    size_t bytesRead_ = 0;

    int writeSocket_     = 0;
    size_t bytesWritten_ = 0;

    size_t messageSize_ = 0;
    std::string id_;

    int bufferSize_ = 0;

    friend std::ostream& operator<<(std::ostream& s, const MultiSocket& socket) {
        socket.print(s);
        return s;
    }
};

}  // namespace eckit::net

#endif
