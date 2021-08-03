/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/cmd/LockCmd.h"

#include "eckit/filesystem/PathName.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

LockCmd::LockCmd() :
    CmdResource("lock,unlock") {}

//----------------------------------------------------------------------------------------------------------------------

LockCmd::~LockCmd() {}

//----------------------------------------------------------------------------------------------------------------------

void LockCmd::execute(std::istream&, std::ostream& out, CmdArg& arg) {
    PathName path("~/locks/admin/cron");

    if (std::string(arg[0]) == "lock") {
        if (path.exists()) {
            out << "already locked (" << path << " exists)" << std::endl;
        }
        else {
            path.touch();
            out << "locked (" << path << " touched)" << std::endl;
            // make the prompt reflect the state
        }
    }
    else  // unlock
    {
        if (path.exists()) {
            path.unlink();
            out << "unlocked (" << path << " removed)" << std::endl;
        }
        else {
            out << "not locked (" << path << " missing)" << std::endl;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

void LockCmd::man(std::ostream&) const {}

//----------------------------------------------------------------------------------------------------------------------

void LockCmd::help(std::ostream&) const {}

//----------------------------------------------------------------------------------------------------------------------

Arg LockCmd::usage(const std::string& cmd) const {
    return Arg();
}

//----------------------------------------------------------------------------------------------------------------------

static LockCmd lockCmd;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
