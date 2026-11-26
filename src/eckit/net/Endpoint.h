// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Simon Smart
/// @author Tiago Quintino
/// @date   May 2019

#ifndef eckit_net_Endpoint_H
#define eckit_net_Endpoint_H

#include <iosfwd>
#include <string>

namespace eckit {

class Stream;
class URI;

namespace net {

//----------------------------------------------------------------------------------------------------------------------

class Endpoint {

public:  // methods

    Endpoint(const URI& uri);      // gets hostname:port from uri
    Endpoint(const std::string&);  // parses the std::string formated as hostname:port
    Endpoint(const std::string& host, int port);
    Endpoint(Stream& s);
    Endpoint();

    virtual ~Endpoint() {}

    virtual const std::string& hostname() const { return host_; }
    const std::string& host() const { return host_; }
    int port() const { return port_; }

    operator std::string() const { return hostname() + ":" + std::to_string(port_); }

    bool operator==(const net::Endpoint& other) const;

    bool operator!=(const net::Endpoint& other) const { return !(*this == other); }

    void print(std::ostream& os) const;
    void encode(Stream& s) const;

protected:  // members

    std::string host_;
    int port_;

private:  // methods

    void validate() const;

    friend std::ostream& operator<<(std::ostream& os, const net::Endpoint& ep) {
        ep.print(os);
        return os;
    }

    friend Stream& operator<<(Stream& s, const net::Endpoint& ep) {
        ep.encode(s);
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace net
}  // namespace eckit

template <>
struct std::hash<eckit::net::Endpoint> {
    std::size_t operator()(const eckit::net::Endpoint& endpoint) const noexcept {
        const std::string& e = endpoint;
        return std::hash<std::string>{}(e);
    }
};

#endif
