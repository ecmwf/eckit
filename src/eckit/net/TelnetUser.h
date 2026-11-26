// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
