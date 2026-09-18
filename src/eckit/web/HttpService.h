// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino

#ifndef eckit_web_HttpService_H
#define eckit_web_HttpService_H

#include "eckit/net/NetService.h"


namespace eckit {


class HttpService : public eckit::net::NetService {
public:

    explicit HttpService(int port, bool visible = false);

    ~HttpService() override;

private:

    eckit::net::NetUser* newUser(eckit::net::TCPSocket&) const override;
    std::string name() const override { return "http"; }
};

}  // namespace eckit


#endif
