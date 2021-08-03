/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

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

void StopCmd::execute(std::istream&, std::ostream& out, CmdArg& arg) {
    std::string app = arg[1];
    bool all        = false;

    if (app == "all")
        all = true;

    Monitor::TaskArray& info = Monitor::instance().tasks();
    for (unsigned long j = 0; j < info.size(); j++)
        if (info[j].busy(true) && (all || app == info[j].application()))
            if (info[j].pid() != getpid())
                info[j].stop();
}

//----------------------------------------------------------------------------------------------------------------------

void StopCmd::help(std::ostream&) const {}

//----------------------------------------------------------------------------------------------------------------------

Arg StopCmd::usage(const std::string& cmd) const {
    return Arg("all") | Arg("<name>", Arg::text);
}

//----------------------------------------------------------------------------------------------------------------------

static StopCmd stopCmd;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
