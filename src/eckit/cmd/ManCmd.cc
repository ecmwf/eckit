/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/cmd/ManCmd.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static ManCmd l;

//----------------------------------------------------------------------------------------------------------------------

ManCmd::ManCmd() :
    CmdResource("man,help") {}

//----------------------------------------------------------------------------------------------------------------------

ManCmd::~ManCmd() {}

//----------------------------------------------------------------------------------------------------------------------

void ManCmd::execute(std::istream&, std::ostream& out, CmdArg& args) {
    CmdResource::help(out, args(1, ""));
}

//----------------------------------------------------------------------------------------------------------------------

void ManCmd::help(std::ostream& out) const {
    out << "manual pages for commands";
}

//----------------------------------------------------------------------------------------------------------------------

Arg ManCmd::usage(const std::string& cmd) const {
    return ~Arg("<command>");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
