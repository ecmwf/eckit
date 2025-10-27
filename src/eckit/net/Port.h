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
/// @author Tiago Quintino
/// @date   Jul 96

#ifndef eckit_net_Port_h
#define eckit_net_Port_h

#include <string>

namespace eckit::net {

class Port {
public:

    Port(const std::string&, int);

    operator int() const { return port_; }

private:

    int port_;
};

}  // namespace eckit::net

#endif
