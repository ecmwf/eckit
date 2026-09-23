// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/cmd/JSONCmd.h"

#include "eckit/log/JSON.h"
#include "eckit/runtime/Monitorable.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

JSONCmd::JSONCmd() : CmdResource("json") {}

JSONCmd::~JSONCmd() {}

void JSONCmd::execute(std::istream&, std::ostream& out, CmdArg& arg) {
    JSON j(out);
    Monitorable::allJSON(j);
}

void JSONCmd::help(std::ostream&) const {}

Arg JSONCmd::usage(const std::string& cmd) const {
    return Arg();
}

static JSONCmd jsonCmd;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
