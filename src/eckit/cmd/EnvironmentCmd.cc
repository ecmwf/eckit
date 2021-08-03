/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/cmd/EnvironmentCmd.h"
#include "eckit/cmd/CmdParser.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

EnvironmentCmd::EnvironmentCmd() :
    CmdResource("environment,env") {}

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
