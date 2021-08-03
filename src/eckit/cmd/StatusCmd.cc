/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/cmd/StatusCmd.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Monitorable.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

StatusCmd::StatusCmd() :
    CmdResource("status") {}

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
