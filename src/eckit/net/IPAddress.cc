/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/net/IPAddress.h"
#include "eckit/exception/Exceptions.h"

#include <ostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

IPAddress::IPAddress(const std::string& addr) {
    if(inet_aton(addr.c_str(), &address_) != 0) {
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

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
