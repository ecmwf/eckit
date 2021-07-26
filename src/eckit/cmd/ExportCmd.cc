/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/cmd/ExportCmd.h"
#include "eckit/cmd/CmdParser.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ExportCmd::ExportCmd() :
    CmdResource("export,setenv") {}

//----------------------------------------------------------------------------------------------------------------------

ExportCmd::~ExportCmd() {}

//----------------------------------------------------------------------------------------------------------------------

void ExportCmd::execute(std::istream&, std::ostream& out, CmdArg& arg) {
    CmdParser::environment(arg[1], arg[2]);
}

//----------------------------------------------------------------------------------------------------------------------

void ExportCmd::help(std::ostream&) const {}

//----------------------------------------------------------------------------------------------------------------------

Arg ExportCmd::usage(const std::string& cmd) const {
    return Arg("<name>") + Arg("<value>");
}

//----------------------------------------------------------------------------------------------------------------------

static ExportCmd exportCmd;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
