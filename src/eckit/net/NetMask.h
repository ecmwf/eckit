// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @date   Dec 2019

#ifndef eckit_net_NetMask_H
#define eckit_net_NetMask_H

#include <iosfwd>
#include <string>

#include "eckit/net/IPAddress.h"

namespace eckit::net {

class NetMask {
public:  // methods

    explicit NetMask(const std::string&);

    bool contains(const IPAddress&) const;

private:  // methods

    IPAddress network_;
    size_t bits_;

    void print(std::ostream& os) const;

    friend std::ostream& operator<<(std::ostream& os, const NetMask& ep) {
        ep.print(os);
        return os;
    }
};

}  // namespace eckit::net

#endif
