/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/cmd/JSONCmd.h"
#include "eckit/log/JSON.h"
#include "eckit/runtime/Monitorable.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

JSONCmd::JSONCmd() :
    CmdResource("json") {}

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
