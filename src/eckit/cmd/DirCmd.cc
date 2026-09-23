// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/cmd/DirCmd.h"

#include <unistd.h>

#include <climits>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

DirCmd::DirCmd() : CmdResource("cd,pwd") {}

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
    if (p) {
        out << p << std::endl;
    }
    else {
        throw FailedSystemCall("getcwd");
    }
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
