// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
