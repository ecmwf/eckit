/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/marsfs/MarsFSService.h"
#include "eckit/filesystem/marsfs/MarsFSUser.h"


namespace eckit {

//-------------------------------------------------------------------------------------------------

MarsFSService::MarsFSService(int port):
	NetService(port)
{
}

MarsFSService::~MarsFSService()
{
}


net::NetUser* MarsFSService::newUser(net::TCPSocket& protocol) const
{
	return new MarsFSUser(protocol);
}


bool MarsFSService::preferToRunAsProcess() const {
    return true;
}

//-------------------------------------------------------------------------------------------------

} // namespace eckit
