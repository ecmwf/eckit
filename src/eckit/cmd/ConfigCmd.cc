// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/cmd/ConfigCmd.h"
#include "eckit/config/Configurable.h"
#include "eckit/config/ResourceMgr.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ConfigCmd::ConfigCmd() : CmdResource("config") {}

ConfigCmd::~ConfigCmd() {}

void ConfigCmd::execute(std::istream&, std::ostream& out, CmdArg& arg) {
    if (arg.exists(1) && arg.exists(2)) {
        ResourceMgr::instance().set(arg[1], arg[2]);
        Configurable::reconfigureAll();
    }

    Configurable::dumpAllResources(out);
}

void ConfigCmd::help(std::ostream& out) const {}

Arg ConfigCmd::usage(const std::string& cmd) const {
    return ~(Arg("<resource>", Arg::text) + Arg("<value>", Arg::text));
}

static ConfigCmd configCmd;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
