/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_net_TelnetUser_h
#define eckit_net_TelnetUser_h

#include "eckit/net/NetUser.h"

namespace eckit::net {

class TelnetUser : public NetUser {
public:

    explicit TelnetUser(net::TCPSocket&);
    ~TelnetUser() override;
    static void terminate(TelnetUser& other) { other.stop(); }

private:

    void serve(Stream&, std::istream&, std::ostream&) override;
    std::string from_;
};

}  // namespace eckit::net

#endif
