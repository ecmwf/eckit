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
