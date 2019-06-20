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
/// @date   May 2019

#ifndef eckit_net_Endpoint_H
#define eckit_net_Endpoint_H

#include <string>
#include <iosfwd>

namespace eckit {

class Stream;

//----------------------------------------------------------------------------------------------------------------------

class Endpoint {

public: // methods

    Endpoint(const std::string& host, int port);
    Endpoint(Stream& s);
    Endpoint();

    const std::string& hostname() const { return hostname_; }
    int port() const { return port_; }

    bool operator==(const Endpoint& other);

    void print(std::ostream& os) const;
    void encode(Stream& s) const;

private: // methods

    friend std::ostream& operator<<(std::ostream& os, const Endpoint& ep) {
        ep.print(os);
        return os;
    }

    friend Stream& operator<<(Stream& s, const Endpoint& ep) {
        ep.encode(s);
        return s;
    }

private: // members

    std::string hostname_;
    int port_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif // eckit_net_Endpoint_H
