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

#include "eckit/cmd/KillCmd.h"
#include "eckit/runtime/Monitor.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

KillCmd::KillCmd() :
    CmdResource("kill") {}


KillCmd::~KillCmd() {}


void KillCmd::execute(std::istream&, std::ostream& out, CmdArg& arg) {

    Monitor::TaskArray& info = Monitor::instance().tasks();

    for (size_t i = 1; i < arg.size(); ++i) {

        Value task(arg[i]);

        if (task.isNil()) {
            // usage(out);
            return;
        }

        if (task.isNumber()) {
            // MarsId or Unix process ID
            unsigned long marsid = (long long)task;
            if (marsid > info.size())
                kill(marsid, out);
            else
                kill(info[marsid].pid(), out);
        }

        if (task.isString()) {
            // Name. Look for Unix process ID
            std::string name = task;
            for (unsigned long i = 0; i < info.size(); ++i)
                if (info[i].busy(true) && info[i].application() == name)
                    kill(info[i].pid(), out);
        }
    }
}


void KillCmd::kill(pid_t pid, std::ostream& out) const {
    static pid_t me = ::getpid();
    if (pid == me)
        out << pid << ": Suicide avoided ;-)" << std::endl;
    else {
        if (::kill(pid, SIGTERM))
            out << Log::syserr << std::endl;
        else
            out << pid << ": Killed" << std::endl;
    }
}


void KillCmd::help(std::ostream& out) const {}


Arg KillCmd::usage(const std::string& cmd) const {
    return Arg("<name>", Arg::text) | Arg("<pid>", Arg::number) | Arg("<task>", Arg::number);
}


static KillCmd killCmd;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
