// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   RemoteCommandUser.h
/// @author Manuel Fuentes
/// @date   Jul 1996

#ifndef eckit_cmd_RemoteCommandUser_H
#define eckit_cmd_RemoteCommandUser_H

#include "eckit/net/NetUser.h"


namespace eckit {


class RemoteCommandUser : public eckit::net::NetUser {
public:

    RemoteCommandUser(eckit::net::TCPSocket&);
    ~RemoteCommandUser() override;
    static void terminate(RemoteCommandUser& other) { other.stop(); }

private:

    void serve(eckit::Stream&, std::istream&, std::ostream&) override;
    std::string from_;
};


}  // namespace eckit

#endif
