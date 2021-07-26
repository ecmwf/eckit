/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <limits.h>
#include <unistd.h>

#include "eckit/cmd/DirCmd.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

DirCmd::DirCmd() :
    CmdResource("cd,pwd") {}

//----------------------------------------------------------------------------------------------------------------------

DirCmd::~DirCmd() {}

//----------------------------------------------------------------------------------------------------------------------

void DirCmd::execute(std::istream&, std::ostream& out, CmdArg& arg) {
    if (arg[0] == "cd") {
        std::string p("~");
        if (arg.exists(1)) {
            p = std::string(arg[1]);
        }
        SYSCALL(chdir(PathName(p).localPath()));
    }

    char buf[PATH_MAX];
    const char* p = getcwd(buf, sizeof(buf));
    if (p)
        out << p << std::endl;
    else
        throw FailedSystemCall("getcwd");
}

//----------------------------------------------------------------------------------------------------------------------

void DirCmd::help(std::ostream&) const {}

//----------------------------------------------------------------------------------------------------------------------

Arg DirCmd::usage(const std::string& cmd) const {
    return cmd == "cd" ? ~Arg("<path>") : Arg();
}

//----------------------------------------------------------------------------------------------------------------------

static DirCmd dirCmd;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
