/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_net_UDPClient_h
#define eckit_net_UDPClient_h

#include <iosfwd>
#include <string>

#include "eckit/memory/NonCopyable.h"


namespace eckit {

class Buffer;
class Configuration;

namespace net {


class UDPClient : private NonCopyable {

public:  // methods

    explicit UDPClient(const Configuration& cfg);
    UDPClient(const std::string& hostname, int port);

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
