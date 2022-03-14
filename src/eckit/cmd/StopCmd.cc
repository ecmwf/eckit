/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <signal.h>
#include <unistd.h>

#include "eckit/cmd/StopCmd.h"
#include "eckit/runtime/Monitor.h"
//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

StopCmd::StopCmd() :
    CmdResource("stop") {}

//----------------------------------------------------------------------------------------------------------------------

StopCmd::~StopCmd() {}

//----------------------------------------------------------------------------------------------------------------------

void StopCmd::execute(std::istream&, std::ostream& out, CmdArg& args) {
    std::string app = args[1];
    bool all        = false;

    bool wait   = args.exists("wait");
    int timeout = 120;

    if (args.exists("timeout")) {
        timeout = int(args["timeout"]);
    }

    if (app == "all") {
        all = true;
    }

    size_t active = 0;

    Monitor::TaskArray& info = Monitor::instance().tasks();

    std::vector<int> signals = {0};
    if (args.exists("kill")) {
        signals = {0, SIGTERM, SIGINT, SIGKILL};
    }

    for (int sig : signals) {

        for (size_t w = 0; w < timeout; w++) {

            active = 0;
            for (size_t j = 0; j < info.size(); j++) {
                if (info[j].busy(true) && (all || app == info[j].application()))
                    if (info[j].pid() != getpid()) {
                        if (sig == 0) {
                            out << "Stopping " << info[j].application() << std::endl;
                            info[j].stop();
                        }
                        else {
                            out << "Killing " << info[j].application() << " with signal " << sig << std::endl;
                            if (::kill(info[j].pid(), sig)) {
                                out << Log::syserr << std::endl;
                            }
                        }
                        active++;
                    }
            }
            if (!wait || !active) {
                break;
            }

            ::sleep(1);
        }
    }

    if (wait || active) {
        throw SeriousBug("Could not stop all tasks");
    }
}

//----------------------------------------------------------------------------------------------------------------------

void StopCmd::help(std::ostream&) const {}

//----------------------------------------------------------------------------------------------------------------------

Arg StopCmd::usage(const std::string& cmd) const {
    return ~Arg("-kill") + ~Arg("-wait") + ~Arg("-timeout") + (Arg("all") | Arg("<name>", Arg::text));
}

//----------------------------------------------------------------------------------------------------------------------

static StopCmd stopCmd;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
