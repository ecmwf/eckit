// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/cmd/StopCmd.h"

#include <unistd.h>

#include <csignal>

#include "eckit/runtime/Monitor.h"
//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

StopCmd::StopCmd() : CmdResource("stop") {}

//----------------------------------------------------------------------------------------------------------------------

StopCmd::~StopCmd() {}

//----------------------------------------------------------------------------------------------------------------------

void StopCmd::execute(std::istream&, std::ostream& out, CmdArg& args) {
    std::string app = args[1];
    bool all        = false;

    bool wait   = args.exists("wait");
    int timeout = 120;

    if (wait) {
        timeout = int(args["wait"]);
    }

    if (app == "all") {
        all = true;
    }

    std::set<std::string> non_stoppable = {"marsadm"};

    Monitor::TaskArray& info = Monitor::instance().tasks();

    std::vector<int> signals = {0};
    if (args.exists("kill")) {
        signals = {0, SIGTERM, SIGINT, SIGKILL};
    }

    std::set<std::string> active;

    for (int sig : signals) {

        for (size_t w = 0; w < timeout; w++) {

            active.clear();

            for (size_t j = 0; j < info.size(); j++) {
                if (info[j].busy(true) && (all || app == info[j].application())) {
                    if (non_stoppable.find(info[j].application()) != non_stoppable.end()) {
                        continue;
                    }
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
                        active.insert(info[j].application());
                    }
                }
            }
            if (!wait || active.size() == 0) {
                break;
            }

            ::sleep(1);
        }
    }

    if (wait && active.size() > 0) {
        std::ostringstream oss;
        oss << "Could not stop task(s): " << active;
        throw SeriousBug(oss.str());
    }
}

//----------------------------------------------------------------------------------------------------------------------

void StopCmd::help(std::ostream&) const {}

//----------------------------------------------------------------------------------------------------------------------

Arg StopCmd::usage(const std::string& cmd) const {
    return ~Arg("-kill") + ~Arg("-wait", Arg::number) + (Arg("all") | Arg("<name>", Arg::text));
}

//----------------------------------------------------------------------------------------------------------------------

static StopCmd stopCmd;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
