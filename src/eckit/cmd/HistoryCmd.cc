// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/eckit.h"

#include "eckit/cmd/HistoryCmd.h"

#include "eckit/cmd/CmdParser.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static HistoryCmd history;

//----------------------------------------------------------------------------------------------------------------------

HistoryCmd::HistoryCmd() : CmdResource("history,h") {}

//----------------------------------------------------------------------------------------------------------------------

HistoryCmd::~HistoryCmd() {}

//----------------------------------------------------------------------------------------------------------------------

void HistoryCmd::execute(std::istream& in, std::ostream& out, CmdArg& args) {
    long long lines = 0;
    if (args.exists(1)) {
        lines = args[1];
    }
    CmdParser::history(lines, out);
}

//----------------------------------------------------------------------------------------------------------------------

void HistoryCmd::help(std::ostream& out) const {
    out << "lists the history of entered commands";
}

//----------------------------------------------------------------------------------------------------------------------

Arg HistoryCmd::usage(const std::string& cmd) const {
    return Arg("<lines>", Arg::number);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
