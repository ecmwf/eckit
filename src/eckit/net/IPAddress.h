/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date   Dec 2019

#ifndef eckit_net_IPAddress_H
#define eckit_net_IPAddress_H

#include <iosfwd>
#include <string>

#include <netinet/in.h>

namespace eckit::net {

class IPAddress {
public:

    // Contructors
    IPAddress(const in_addr& address) : address_(address) {}

    IPAddress(const std::string&);
    IPAddress(const char*);

    std::string asString() const;

    const in_addr& address() const { return address_; }

    static IPAddress myIPAddress();
    static IPAddress hostAddress(const std::string& hostname);

    bool operator==(const IPAddress& other) const;

private:

    // Members

    in_addr address_;

    // Methods

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const IPAddress& p) {
        p.print(s);
        return s;
    }
};

}  // namespace eckit::net

#endif
