/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/cmd/UpTimeCmd.h"
#include "eckit/log/Seconds.h"
#include "eckit/log/TimeStamp.h"
#include "eckit/runtime/Monitor.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

UpTimeCmd::UpTimeCmd() :
    CmdResource("uptime") {}

//----------------------------------------------------------------------------------------------------------------------

UpTimeCmd::~UpTimeCmd() {}

//----------------------------------------------------------------------------------------------------------------------

void UpTimeCmd::execute(std::istream&, std::ostream& out, CmdArg& arg) {
    time_t now = ::time(0);

    Monitor::TaskArray& info = Monitor::instance().tasks();
    for (unsigned long j = 0; j < info.size(); j++)
        if (info[j].busy(true) && (info[j].parent() == -1))
            out << TimeStamp(info[j].start()) << " " << info[j].application() << " " << Seconds(now - info[j].start())
                << std::endl;
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
