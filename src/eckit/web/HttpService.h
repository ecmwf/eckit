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
