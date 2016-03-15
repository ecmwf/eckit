/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/eckit_config.h"

#include "eckit/cmd/HistoryCmd.h"

#ifdef READLINE_FOUND
#include <readline/readline.h>
#include <readline/history.h>
#else
#include "eckit/cmd/CmdParser.h"
#endif

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

static HistoryCmd history;

//-----------------------------------------------------------------------------

HistoryCmd::HistoryCmd() : CmdResource("history,h") {
}

//-----------------------------------------------------------------------------

HistoryCmd::~HistoryCmd() {
}

//-----------------------------------------------------------------------------

void HistoryCmd::execute(std::istream& in, std::ostream& out, CmdArg& args) {
    long long lines = 0;
    if (args.exists(1)) lines = args[1];
#ifdef READLINE_FOUND
    HIST_ENTRY* h;
    int n = history_base;
    if (lines) {
        n += history_length - lines;
    }
    while ((h = history_get(n)) != 0) {
        std::cout << std::setw(4) << std::setfill(' ') << n << " " << h->line << std::endl;
        n++;
    }
#else
    CmdParser::history(lines, out);
#endif
}

//-----------------------------------------------------------------------------

void HistoryCmd::help(std::ostream& out) const {
    out << "lists the history of entered commands";
}

//-----------------------------------------------------------------------------

Arg HistoryCmd::usage(const std::string& cmd) const {
    return Arg("<lines>", Arg::number);
}

//-----------------------------------------------------------------------------

} // namespace eckit
