/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
