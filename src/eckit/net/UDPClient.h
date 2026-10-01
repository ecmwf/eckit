// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_net_UDPClient_h
#define eckit_net_UDPClient_h

#include <iosfwd>
#include <string>


namespace eckit {

class Buffer;
class Configuration;

namespace net {


class UDPClient {

public:  // methods

    explicit UDPClient(const Configuration& cfg);
    UDPClient(const std::string& hostname, int port);

    UDPClient(const UDPClient&)            = delete;
    UDPClient& operator=(const UDPClient&) = delete;
    UDPClient(UDPClient&&)                 = delete;
    UDPClient& operator=(UDPClient&&)      = delete;

    ~UDPClient();

    void send(const void* buf, size_t length);

protected:  // methods

    void print(std::ostream& s) const;

    friend std::ostream& operator<<(std::ostream& s, const UDPClient& socket) {
        socket.print(s);
        return s;
    }

private:  // members

    std::string hostname_;

    int port_;

    int socketfd_;

    struct addrinfo* servinfo_;
    struct addrinfo* addr_;

    void init();
};


}  // namespace net
}  // namespace eckit


#endif
