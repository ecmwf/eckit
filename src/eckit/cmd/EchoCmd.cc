/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/cmd/EchoCmd.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

EchoCmd::EchoCmd() :
    CmdResource("echo") {}

//----------------------------------------------------------------------------------------------------------------------

EchoCmd::~EchoCmd() {}

//----------------------------------------------------------------------------------------------------------------------

void EchoCmd::execute(std::istream&, std::ostream& out, CmdArg& arg) {
    std::string sep = "";
    for (size_t i = 1; i < arg.size(); i++) {
        Value var = arg[i];  // Otherwise is Accessor<>.print()
        out << sep << var;
        sep = " ";
    }
    out << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

void EchoCmd::help(std::ostream&) const {}

//----------------------------------------------------------------------------------------------------------------------

Arg EchoCmd::usage(const std::string& cmd) const {
    return Arg("<any> ...");
}

//----------------------------------------------------------------------------------------------------------------------

static EchoCmd echoCmd;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
