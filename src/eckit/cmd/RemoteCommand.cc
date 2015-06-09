/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/cmd/CmdResource.h"
#include "eckit/cmd/RemoteCommand.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/config/Resource.h"
#include "eckit/serialisation/Stream.h"

//-----------------------------------------------------------------------------

namespace eckit {

// =======================================================================

RemoteCommandUser::RemoteCommandUser(TCPSocket& protocol) : NetUser(protocol), from_(protocol_.remoteHost()) {
}

RemoteCommandUser::~RemoteCommandUser() {
}

void RemoteCommandUser::serve(eckit::Stream& s, std::istream& in, std::ostream& out) {

    Log::debug() << "Starting a remote command connection " << std::endl;

    Monitor::instance().kind("monitor");
    Monitor::instance().name("monitor");

    CmdArg cmd(s);

    Log::debug() << "Got command: " << cmd << std::endl;
    //	CmdResource::dispatch(in,out);
    CmdResource::run(CmdResource::command, cmd, in, out);

    Log::debug() << "Exiting remote command ..." << std::endl;
}

// =======================================================================

RemoteCommander::RemoteCommander(int p) : NetService(p) {
    Monitor::instance().port(port());
    std::string host = eckit::Resource<std::string>("localBindingAddr", "localHost");
    Monitor::instance().host(host);
}

RemoteCommander::~RemoteCommander() {
}

NetUser* RemoteCommander::newUser(TCPSocket& protocol) {
    return new RemoteCommandUser(protocol);
}

// =======================================================================

RemoteCommandable::RemoteCommandable(int port) : commander_(new RemoteCommander(port)) {
    commander_.start();
}

RemoteCommandable::~RemoteCommandable() {
    commander_.stop();
}

//-----------------------------------------------------------------------------

}  // namespace eckit
