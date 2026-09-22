// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/net/IPAddress.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <ostream>

#include "eckit/exception/Exceptions.h"

namespace eckit::net {

IPAddress::IPAddress(const std::string& addr) {
    if (inet_aton(addr.c_str(), &address_) == 0) {
        std::ostringstream os;
        os << "Invalid IP address [" << addr << "]";
        throw eckit::SeriousBug(os.str());
    }
}

IPAddress::IPAddress(const char* addr) {
    if (inet_aton(addr, &address_) == 0) {
        std::ostringstream os;
        os << "Invalid IP address [" << addr << "]";
        throw eckit::SeriousBug(os.str());
    }
}

void IPAddress::print(std::ostream& os) const {
    os << inet_ntoa(address_);
}


std::string IPAddress::asString() const {
    return inet_ntoa(address_);
}

bool IPAddress::operator==(const IPAddress& other) const {
    return ::memcmp(&address_, &other.address_, sizeof(address_)) == 0;
}


IPAddress IPAddress::hostAddress(const std::string& hostname) {
    struct hostent* hostEntry = gethostbyname(hostname.c_str());
    ASSERT(hostEntry);

    return IPAddress(inet_ntoa(*((struct in_addr*)hostEntry->h_addr_list[0])));
}

IPAddress IPAddress::myIPAddress() {
    static bool done = false;
    static IPAddress mine("255.255.255.255");

    if (!done) {
        char hostname[256] = {};
        SYSCALL(::gethostname(hostname, sizeof(hostname) - 1));

        struct hostent* entry = gethostbyname(hostname);
        ASSERT(entry);

        mine = IPAddress(inet_ntoa(*((struct in_addr*)entry->h_addr_list[0])));
        done = true;
    }

    return mine;
}

}  // namespace eckit::net
