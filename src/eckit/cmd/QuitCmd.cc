/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/cmd/QuitCmd.h"
#include "eckit/runtime/Application.h"
#include "eckit/runtime/Main.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static QuitCmd quit;

//----------------------------------------------------------------------------------------------------------------------

QuitCmd::QuitCmd() :
    CmdResource("quit,exit") {}

//----------------------------------------------------------------------------------------------------------------------

QuitCmd::~QuitCmd() {}

//----------------------------------------------------------------------------------------------------------------------

void QuitCmd::execute(std::istream&, std::ostream& out, CmdArg&) {
    out << "Bye" << std::endl;
    Application::instance().terminate();
}

//----------------------------------------------------------------------------------------------------------------------

void QuitCmd::help(std::ostream& out) const {
    out << "quits the '" << Main::instance().name() << "'";
}

//----------------------------------------------------------------------------------------------------------------------

Arg QuitCmd::usage(const std::string& cmd) const {
    return Arg();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
