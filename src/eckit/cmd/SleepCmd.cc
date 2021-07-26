/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <unistd.h>

#include "eckit/cmd/SleepCmd.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

SleepCmd::SleepCmd() :
    CmdResource("sleep") {}

//----------------------------------------------------------------------------------------------------------------------

SleepCmd::~SleepCmd() {}

//----------------------------------------------------------------------------------------------------------------------

void SleepCmd::execute(std::istream&, std::ostream& out, CmdArg& arg) {
    long t = arg[1];

    ::sleep(t);
}

//----------------------------------------------------------------------------------------------------------------------

void SleepCmd::help(std::ostream&) const {}

//----------------------------------------------------------------------------------------------------------------------

Arg SleepCmd::usage(const std::string& cmd) const {
    return Arg("<duration>", Arg::number);
}

//----------------------------------------------------------------------------------------------------------------------

static SleepCmd sleepCmd;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
