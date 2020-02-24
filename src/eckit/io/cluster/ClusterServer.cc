/*
 * (C) Copyright 1996- ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/io/cluster/ClusterDisks.h"
#include "eckit/io/cluster/ClusterNodes.h"
#include "eckit/io/cluster/ClusterServer.h"
#include "eckit/io/cluster/ClusterService.h"

namespace eckit {

//-------------------------------------------------------------------------------------------------

ClusterServer::ClusterServer(int port):
	cluster_(new ClusterService(port))
{
    ClusterNodes::reset();
    ClusterDisks::reset();

    cluster_.start();
}

ClusterServer::~ClusterServer()
{
	cluster_.stop();
}

//-------------------------------------------------------------------------------------------------

} // namespace eckit
