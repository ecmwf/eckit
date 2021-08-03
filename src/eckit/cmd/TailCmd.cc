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

#include "eckit/cmd/TailCmd.h"
#include "eckit/config/Resource.h"
#include "eckit/os/SignalHandler.h"
#include "eckit/runtime/Monitor.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static TailCmd tail;

//----------------------------------------------------------------------------------------------------------------------

TailCmd::TailCmd() :
    CmdResource("tail") {}

//----------------------------------------------------------------------------------------------------------------------

TailCmd::~TailCmd() {}

//----------------------------------------------------------------------------------------------------------------------

void TailCmd::execute(std::istream&, std::ostream& out, CmdArg& args) {
    long long pid   = -1;
    long long lines = Resource<long>("tailCmd", 10);
    bool follow     = false;

    if (args.exists(1))
        pid = args[1];

    if (args.exists("f")) {
        follow = true;
        pid    = args["f"];
    }

    if (args.exists("n")) {
        lines = args["n"];
    }

    if (pid == -1) {
        // usage(out); out << std::endl;
        return;
    }

    Monitor::TaskArray& info = Monitor::instance().tasks();

    if (!info[pid].busy(true)) {
        out << pid << ": No such process" << std::endl;
        return;
    }

    unsigned long where = 0;

    static char buffer[10240];

    unsigned long pos = where;
    unsigned long len = info[pid].text(buffer, sizeof(buffer), pos);

    if (len) {
        long nl     = 0;
        buffer[len] = 0;
        int j       = 0;
        for (j = (buffer[len - 1] == '\n') ? len - 2 : len; j >= 0 && nl < lines; j--)
            if (buffer[j] == '\n')
                nl++;

        if (j != 0)
            j++;

        out << &(buffer[j]) << std::flush;
        where = pos;
    }

    SignalHandler controlC;

    time_t last = info[pid].last();
    while (info[pid].busy(true) && follow) {
        if (last != info[pid].last()) {
            pos = where;
            len = info[pid].text(buffer, sizeof(buffer), pos);
            if (len) {
                buffer[len] = 0;
                out << buffer << std::flush;
                where = pos;
            }
            last = info[pid].last();
        }
        else
            ::usleep(1000);
    }
}

//----------------------------------------------------------------------------------------------------------------------

void TailCmd::help(std::ostream& out) const {
    out << "as the UNIX counterpart";
}

//----------------------------------------------------------------------------------------------------------------------

Arg TailCmd::usage(const std::string& cmd) const {
    return ~Arg("-f") + Arg("<task>", Arg::number);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
