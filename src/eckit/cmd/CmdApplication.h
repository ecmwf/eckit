/*
 * (C) Copyright 1996-2016 ECMWF.
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

//-----------------------------------------------------------------------------

#ifdef ECKIT_HAVE_READLINE

#include <readline/readline.h>
#include <readline/history.h>

#include "eckit/cmd/CmdResource.h"
#include "eckit/parser/Tokenizer.h"

static std::vector<std::string> completion;
static size_t n = 0;
static int st, en;

static char* get_next_match(const char* text, int state) {
    if (state == 0) {
        completion = eckit::CmdResource::completion(text);
        eckit::CmdParser::aliasCompletion(text, completion);
        n = 0;
    }

    if (n < completion.size()) {
        return strdup(completion[n++].c_str());
    }

    return 0;
}

static char* get_next_arg(const char* text, int state) {
    if (state == 0) {

        eckit::Tokenizer parse(" ");
        char* t = rl_copy_text(0, en);
        std::vector<std::string> s;
        parse(t, s);
        ::free(t);

        completion = eckit::CmdResource::completion(s);

        n = 0;
    }

    if (n < completion.size()) {
        return strdup(completion[n++].c_str());
    }

    return 0;
}

static char** cmd_application_completion(const char* text, int start, int end) {
    char** matches = 0;
    st = start;
    en = end;

    if (start == 0) {
        matches = rl_completion_matches(text, get_next_match);
    } else {
        matches = rl_completion_matches(text, get_next_arg);
    }
    return matches;
}

#endif  // ECKIT_HAVE_READLINE

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

template <class App>
class CmdApplication : public App {
public:
    CmdApplication(int argc, char** argv);
    ~CmdApplication();

private:
    CmdApplication(const CmdApplication&);
    CmdApplication& operator=(const CmdApplication&);

    virtual void run();
    void startup(std::ostream&);

    void serveMode(long);
    void userMode();
};

//-----------------------------------------------------------------------------

template <class App>
CmdApplication<App>::CmdApplication(int argc, char** argv) : App(argc, argv) {
}

//-----------------------------------------------------------------------------

template <class App>
CmdApplication<App>::~CmdApplication() {
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

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

#ifdef ECKIT_HAVE_READLINE

        PathName lock("~/locks/admin/cron");
        rl_readline_name = App::name().c_str();
        rl_attempted_completion_function = cmd_application_completion;

        using_history();

        if (interactive) {
            PathName history("~/." + App::name() + "_history");
            long size = eckit::Resource<long>("historySize", 100);
            if (history.exists()) {
                if (read_history(history.localPath())) Log::error() << history << ": " << Log::syserr << std::endl;
            }

            stifle_history(size);

            char* p;
            std::string last;
            Log::status() << "Idle..." << std::endl;
            const std::string prompt = App::name() + "> ";
            const std::string locked_prompt = App::name() + " (LOCKED)> ";
            while ((p = readline((lock.exists() ? locked_prompt : prompt).c_str())) != 0) {
                char* q = 0;
                switch (history_expand(p, &q)) {
                    case 0:
                        break;

                    case 1:
                        ::free(p);
                        p = strdup(q);
                        std::cout << p << std::endl;
                        break;

                    case 2:
                        std::cout << q << std::endl;
                        *p = 0;
                        break;

                    case -1:
                        Log::warning() << q << std::endl;
                        break;
                }

                std::string line(p);
                if (*p && last != line) {
                    last = line;
                    add_history(p);
                    if (write_history(history.localPath())) Log::error() << history << ": " << Log::syserr << std::endl;
                }
                ::free(p);

                try {
                    CmdParser::parse(line, std::cout);
                } catch (std::exception& e) {
                    if (fail) throw e;

                    Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
                    Log::error() << "** Exception is ignored" << std::endl;
                }
                Log::status() << "Idle..." << std::endl;
            }

        } else
#endif  // ECKIT_HAVE_READLINE
        while (std::cin) {
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

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

template <class App>
void CmdApplication<App>::run() {
    long port = Resource<long>("-serve", 0);
    if (port)
        serveMode(port);
    else
        userMode();
}

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
