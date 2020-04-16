/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   CmdApplication.cc
/// @author Florian Rathgeber
/// @date   June 2015

#include <unistd.h>
#include <fstream>

#include "eckit/cmd/CmdApplication.h"
#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/SockBuf.h"
#include "eckit/net/TCPServer.h"
#include "eckit/runtime/Main.h"
#include "eckit/runtime/Monitor.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

CmdApplication::CmdApplication() {}

CmdApplication::~CmdApplication() {}

std::string CmdApplication::prompt() const {
    return name() + "% ";
}

std::string CmdApplication::name() const {
    return Main::instance().name();
}

void CmdApplication::startup(std::ostream& out) {
    StringList rc;

    rc.push_back("~/." + name() + "rc");
    rc.push_back("~/etc/" + name() + "rc");

    for (StringList::const_iterator it = rc.begin(); it != rc.end(); ++it) {
        PathName path(*it);
        if (path.exists()) {
            Log::info() << "Startup " << path << std::endl;
            std::ifstream in(path.localPath());
            CmdParser::parse(in, out, *this);
        }
    }
}

void CmdApplication::userMode() {
    std::string command = Resource<std::string>("-command", "");

    Monitor::instance().stoppable(false);

    CmdParser::prompt(false);
    startup(std::cout);

    PathName file = Resource<PathName>("-f", "");
    bool fail     = Resource<bool>("-fail", false);

    if (command != "") {
        Log::info() << "command: " << command << std::endl;
        try {
            CmdParser::parse(command, std::cout);
        }
        catch (std::exception& e) {
            if (fail)
                throw;

            Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
            Log::error() << "** Exception is ignored" << std::endl;
        }
        return;
    }

    bool interactive = Resource<bool>("-interactive", ::isatty(1) || ::isatty(0));

    CmdParser::prompt(::isatty(1));
    CmdParser::prompt(interactive);
    if (file.exists()) {
        std::ifstream in(file.localPath());
        CmdParser::parse(in, std::cout, *this);
    }
    else {
        PathName home("~");
        Log::info() << name() << " home is " << home << std::endl;

        CmdParser::console(interactive && ::isatty(0) && ::isatty(1));

        while (std::cin) {
            try {
                CmdParser::parse(std::cin, std::cout, *this);
                break;
            }
            catch (std::exception& e) {
                if (fail)
                    throw;

                Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
                Log::error() << "** Exception is ignored" << std::endl;
            }
        }
    }
}

void CmdApplication::serveMode(long port) {
    Log::info() << "Offering remote commands on port " << port << std::endl;

    net::TCPServer server(port);

    for (;;) {
        net::TCPSocket p = server.accept();
        Log::info() << "Connection from " << p.remoteHost() << std::endl;

        SockBuf buf(p);
        std::ostream out(&buf);
        std::istream in(&buf);

        out << "Welcome " << p.remoteHost() << std::endl;

        CmdParser::parse(in, out, *this);
    }
}

void CmdApplication::execute() {
    long port = Resource<long>("-serve", 0);
    if (port)
        serveMode(port);
    else
        userMode();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
