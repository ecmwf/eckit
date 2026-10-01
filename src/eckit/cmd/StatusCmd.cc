// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/cmd/StatusCmd.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Monitorable.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

StatusCmd::StatusCmd() : CmdResource("status") {}

StatusCmd::~StatusCmd() {}

void StatusCmd::execute(std::istream&, std::ostream& out, CmdArg&) {
    format(out, Log::monitorFormat);
    Monitorable::allStatuses(out);
}

void StatusCmd::help(std::ostream&) const {}

Arg StatusCmd::usage(const std::string&) const {
    return Arg();
}

static StatusCmd statusCmd;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
