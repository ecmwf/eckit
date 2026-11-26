// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/cmd/UpTimeCmd.h"
#include "eckit/log/Seconds.h"
#include "eckit/log/TimeStamp.h"
#include "eckit/runtime/Monitor.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

UpTimeCmd::UpTimeCmd() : CmdResource("uptime") {}

//----------------------------------------------------------------------------------------------------------------------

UpTimeCmd::~UpTimeCmd() {}

//----------------------------------------------------------------------------------------------------------------------

void UpTimeCmd::execute(std::istream&, std::ostream& out, CmdArg& arg) {
    time_t now = ::time(nullptr);

    Monitor::TaskArray& info = Monitor::instance().tasks();
    for (unsigned long j = 0; j < info.size(); j++) {
        if (info[j].busy(true) && (info[j].parent() == -1)) {
            out << TimeStamp(info[j].start()) << " " << info[j].application() << " " << Seconds(now - info[j].start())
                << std::endl;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

void UpTimeCmd::help(std::ostream&) const {}

//----------------------------------------------------------------------------------------------------------------------

Arg UpTimeCmd::usage(const std::string& cmd) const {
    return Arg();
}

//----------------------------------------------------------------------------------------------------------------------

static UpTimeCmd uptimeCmd;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
