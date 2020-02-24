/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult (Oct 96)
/// @author Simon Smart (Feb 2020)

#ifndef eckit_io_cluster_ClusterService_h
#define eckit_io_cluster_ClusterService_h

#include "eckit/net/NetService.h"

namespace eckit {

//-------------------------------------------------------------------------------------------------

class ClusterService : public eckit::net::NetService {
public:

    ClusterService(int port);
	~ClusterService();

    // No copy allowed
    ClusterService(const ClusterService&) = delete;
    ClusterService& operator=(const ClusterService&) = delete;

private:

    eckit::net::NetUser* newUser(eckit::net::TCPSocket&) const override;
    std::string   name() const override { return "cluster"; }

};

//-------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
