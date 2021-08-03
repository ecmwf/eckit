/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/cmd/RemoteCmd.h"
#include "eckit/net/TCPClient.h"
#include "eckit/net/TCPStream.h"
#include "eckit/runtime/Monitor.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

RemoteCmd::RemoteCmd() :
    CmdResource("remote") {}

RemoteCmd::~RemoteCmd() {}

void RemoteCmd::execute(std::istream&, std::ostream& out, CmdArg& arg) {
    Monitor::TaskArray& info = Monitor::instance().tasks();

    Value a(arg[1]);
    long taskid = -1;
    int port    = 0;
    std::string host;

    if (a.isNil()) {
        out << "An application or task id must be specified" << std::endl;
        return;
    }

    if (a.isString()) {
        std::string name = arg[1];
        for (unsigned long j = 0; j < info.size(); j++) {
            if (info[j].busy(true) && info[j].application() == name && info[j].parent() == -1) {
                taskid = j;
                break;
            }
        }
    }

    if (a.isNumber())
        taskid = (long long)a;

    if (taskid < 0 || static_cast<unsigned long>(taskid) >= info.size()) {
        if (a.isString())
            out << a << ": task not found" << std::endl;
        if (a.isNumber())
            out << taskid << ": task Id not found" << std::endl;
        return;
    }

    if (info[taskid].busy(true)) {
        port = info[taskid].port();
        host = info[taskid].host();
    }

    if (port == 0 || host == "") {
        out << "Application '" << taskid << "' cannot be reached" << std::endl;
        return;
    }

    net::TCPStream remote(net::TCPClient().connect(host, port));

    Log::debug() << "RemoteCommand to " << host << ":" << port << std::endl;

    try {
        CmdArg remotecmd = arg;
        remotecmd.erase(0, 1);
        remotecmd.encode(remote);

        char buf[1024 * 4];
        int len = 1024 * 4;
        while ((len = remote.read(buf, sizeof(buf))) > 0)
            std::copy(buf, buf + len, std::ostream_iterator<char>(out, ""));
        out << std::flush;
    }
    catch (std::exception& e) {
        // Suppose the server has finished
    }
}


void RemoteCmd::help(std::ostream&) const {}


Arg RemoteCmd::usage(const std::string& cmd) const {
    return Arg("<process>") + Arg("<command>...");
}

static RemoteCmd remoteCmd;

//----------------------------------------------------------------------------------------------------------------------


}  // namespace eckit
