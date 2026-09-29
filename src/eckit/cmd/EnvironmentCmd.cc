// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/cmd/EnvironmentCmd.h"
#include "eckit/cmd/CmdParser.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

EnvironmentCmd::EnvironmentCmd() : CmdResource("environment,env") {}

//----------------------------------------------------------------------------------------------------------------------

EnvironmentCmd::~EnvironmentCmd() {}

//----------------------------------------------------------------------------------------------------------------------

void EnvironmentCmd::execute(std::istream&, std::ostream& out, CmdArg& arg) {
    CmdParser::environment(out);
}

//----------------------------------------------------------------------------------------------------------------------

void EnvironmentCmd::help(std::ostream&) const {}

//----------------------------------------------------------------------------------------------------------------------

Arg EnvironmentCmd::usage(const std::string& cmd) const {
    return Arg();
}

//----------------------------------------------------------------------------------------------------------------------

static EnvironmentCmd environmentCmd;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
