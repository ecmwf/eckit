/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/io/cluster/ClusterService.h"

#include "eckit/io/cluster/ClusterUser.h"
#include "eckit/net/SocketOptions.h"


namespace eckit {

//-------------------------------------------------------------------------------------------------

ClusterService::ClusterService(int port):
    NetService(port)
{
}

ClusterService::~ClusterService()
{
}


net::NetUser* ClusterService::newUser(net::TCPSocket& protocol) const
{
	return new ClusterUser(protocol);
}

//-------------------------------------------------------------------------------------------------

} // namespace eckit

