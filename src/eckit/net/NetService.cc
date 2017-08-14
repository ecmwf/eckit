/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/log/Log.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/net/NetService.h"
#include "eckit/net/NetUser.h"
#include "eckit/thread/ThreadControler.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

NetService::NetService(int port, bool visible):
    server_(port),
    visible_(visible)
{
}

NetService::~NetService()
{
}

std::string NetService::hostname() const {
    return server_.localHost();
}

int NetService::port() const {
    return server_.localPort();
}

void NetService::run()
{
    Monitor::instance().show(visible_);
	Monitor::instance().name(name());
	Monitor::instance().kind(name());

    Log::status() << "Waiting on port " << port() << std::endl;

	while(!stopped())
	{
		ThreadControler t(newUser(server_.accept()));
		t.start();
	}
}


//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

