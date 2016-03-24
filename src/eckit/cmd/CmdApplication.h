/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   CmdApplication.h
/// @author Florian Rathgeber
/// @date   June 2015

#ifndef eckit_cmd_CmdApplication_H
#define eckit_cmd_CmdApplication_H

#include "eckit/eckit_config.h"
#include "eckit/cmd/CmdParser.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/SockBuf.h"
#include "eckit/config/Resource.h"
#include "eckit/net/TCPServer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template <class App>
class CmdApplication : public App {
public:
    CmdApplication(int argc, char** argv);
    ~CmdApplication();

private:
    CmdApplication(const CmdApplication&);
    CmdApplication& operator=(const CmdApplication&);

    virtual std::string prompt() const;

    virtual void run();

    void startup(std::ostream&);

    void serveMode(long);
    void userMode();
};

//----------------------------------------------------------------------------------------------------------------------

template <class App>
CmdApplication<App>::CmdApplication(int argc, char** argv) : App(argc, argv) {
}


template <class App>
CmdApplication<App>::~CmdApplication() {
}

template <class App>
std::string CmdApplication<App>::prompt() {
    return this->App::appName();
}

template <class App>
void CmdApplication<App>::startup(std::ostream& out) {
    StringList rc;

    rc.push_back("~/." + App::instance().appName() + "rc");
    rc.push_back("~/etc/" + App::instance().appName() + "rc");

    for (StringList::const_iterator it = rc.begin(); it != rc.end(); ++it) {
        PathName path(*it);
        if (path.exists()) {
            Log::info() << "Startup " << path << std::endl;
            std::ifstream in(path.localPath());
            CmdParser::parse(in, out);
        }
    }
}


template <class App>
void CmdApplication<App>::userMode() {
    std::string command = Resource<std::string>("-command", "");

    Monitor::instance().stoppable(false);

    CmdParser::prompt(false);
    startup(std::cout);

    PathName file = Resource<PathName>("-f", "");
    bool fail = Resource<bool>("-fail", false);

    if (command != "") {
        Log::info() << "command: " << command << std::endl;
        try {
            CmdParser::parse(command, std::cout);
        } catch (std::exception& e) {
            if (fail) throw e;

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
        CmdParser::parse(in, std::cout);
    } else {

        PathName home("~");
        Log::info() << App::name() << " home is " << home << std::endl;

        while (std::cin) {
            CmdParser::prompt( prompt() );
            try {
                CmdParser::parse(std::cin, std::cout);
            } catch (std::exception& e) {
                if (fail) throw e;

                Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
                Log::error() << "** Exception is ignored" << std::endl;
            }
        }
    }
}


template <class App>
void CmdApplication<App>::serveMode(long port) {
    Log::info() << "Offering remote commands on port " << port << std::endl;

    TCPServer server(port);

    for (;;) {
        TCPSocket p = server.accept();
        Log::info() << "Connection from " << p.remoteHost() << std::endl;

        SockBuf buf(p);
        std::ostream out(&buf);
        std::istream in(&buf);

        out << "Welcome " << p.remoteHost() << std::endl;

        CmdParser::parse(in, out);
    }
}


template <class App>
void CmdApplication<App>::run() {
    long port = Resource<long>("-serve", 0);
    if (port)
        serveMode(port);
    else
        userMode();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
