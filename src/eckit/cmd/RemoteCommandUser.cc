// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/cmd/RemoteCommandUser.h"

#include "eckit/cmd/CmdResource.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/serialisation/Stream.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

RemoteCommandUser::RemoteCommandUser(net::TCPSocket& protocol) :
    net::NetUser(protocol), from_(protocol_.remoteHost()) {}

//----------------------------------------------------------------------------------------------------------------------

RemoteCommandUser::~RemoteCommandUser() {}

//----------------------------------------------------------------------------------------------------------------------

void RemoteCommandUser::serve(Stream& s, std::istream& in, std::ostream& out) {

    Log::debug() << "Starting a remote command connection " << std::endl;

    Monitor::instance().kind("monitor");
    Monitor::instance().name("monitor");

    CmdArg cmd(s);

    Log::debug() << "Got command: " << cmd << std::endl;
    //	CmdResource::dispatch(in,out);
    CmdResource::run(CmdResource::command, cmd, in, out);

    Log::debug() << "Exiting remote command ..." << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
