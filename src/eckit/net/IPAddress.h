// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
