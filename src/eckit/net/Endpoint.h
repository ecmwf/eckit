/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @author Tiago Quintino
/// @date   May 2019

#ifndef eckit_net_Endpoint_H
#define eckit_net_Endpoint_H

#include <iosfwd>
#include <string>

namespace eckit {

class Stream;

namespace net {

//----------------------------------------------------------------------------------------------------------------------

class Endpoint {

public:                            // methods
    Endpoint(const std::string&);  // parses the std::string formated as hostname:port
    Endpoint(const std::string& host, int port);
    Endpoint(Stream& s);
    Endpoint();

    const std::string& hostname() const { return host_; }
    const std::string& host() const { return host_; }
    int port() const { return port_; }

    bool operator==(const net::Endpoint& other);

    void print(std::ostream& os) const;
    void encode(Stream& s) const;

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

private:  // members
    std::string host_;
    int port_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace net
}  // namespace eckit


#endif
