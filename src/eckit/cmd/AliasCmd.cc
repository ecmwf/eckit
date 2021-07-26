/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/cmd/AliasCmd.h"
#include "eckit/cmd/CmdParser.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

AliasCmd::AliasCmd() :
    CmdResource("alias") {}

//----------------------------------------------------------------------------------------------------------------------

AliasCmd::~AliasCmd() {}

//----------------------------------------------------------------------------------------------------------------------

void AliasCmd::execute(std::istream&, std::ostream& out, CmdArg& arg) {
    if (arg.exists(2))
        CmdParser::alias(arg[1], arg[2]);
    else if (arg.exists(1))
        CmdParser::alias(arg[1]);
    else
        CmdParser::alias();
}

//----------------------------------------------------------------------------------------------------------------------

void AliasCmd::help(std::ostream&) const {}

//----------------------------------------------------------------------------------------------------------------------

Arg AliasCmd::usage(const std::string& cmd) const {
    return Arg("<name>") + Arg("<value>");
}

//----------------------------------------------------------------------------------------------------------------------

static AliasCmd aliasCmd;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
