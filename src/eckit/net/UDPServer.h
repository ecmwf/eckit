// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_net_UDPServer_h
#define eckit_net_UDPServer_h

#include <iosfwd>
#include <string>


namespace eckit {

class Buffer;

namespace net {


class UDPServer {

public:  // methods

    explicit UDPServer(int port);

    UDPServer(const UDPServer&)            = delete;
    UDPServer& operator=(const UDPServer&) = delete;
    UDPServer(UDPServer&&)                 = delete;
    UDPServer& operator=(UDPServer&&)      = delete;

    ~UDPServer();

    size_t receive(void* buf, long length);
    size_t receive(eckit::Buffer&);

protected:  // methods

    void print(std::ostream& s) const;

    friend std::ostream& operator<<(std::ostream& s, const UDPServer& socket) {
        socket.print(s);
        return s;
    }

    std::string remoteHost(struct sockaddr_storage&) const;

private:  // members

    int port_;

    int socketfd_;
};


}  // namespace net
}  // namespace eckit


#endif
