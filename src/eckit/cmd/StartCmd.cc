/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <fstream>

#include "eckit/cmd/StartCmd.h"
#include "eckit/config/Resource.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/utils/Tokenizer.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

StartCmd::StartCmd() :
    CmdResource("start") {}

//----------------------------------------------------------------------------------------------------------------------

StartCmd::~StartCmd() {}

//----------------------------------------------------------------------------------------------------------------------

void StartCmd::execute(std::istream&, std::ostream& out, CmdArg& arg) {
    std::string app = arg["1"];

    if (app == "all") {

        PathName path("~/etc/applications");
        std::vector<std::string> names;

        if (path.exists()) {
            std::ifstream in(path.localPath());
            std::string line;
            while (in >> line) {
                names.push_back(line);
            }
        }
        else {
            std::string all = Resource<std::string>("allApplications", "mars,reader,flusher,cleaner,httpsvr,safety");

            Tokenizer token(", \t");
            token(all, names);
        }

        for (std::vector<std::string>::iterator i = names.begin(); i != names.end(); ++i)
            start(out, *i);
    }
    else
        start(out, app);
}

//----------------------------------------------------------------------------------------------------------------------

void StartCmd::start(std::ostream& out, const std::string& app) const {
    Monitor::instance().start(app);
}

//----------------------------------------------------------------------------------------------------------------------

void StartCmd::help(std::ostream&) const {}

//----------------------------------------------------------------------------------------------------------------------

Arg StartCmd::usage(const std::string& cmd) const {
    return Arg("all") | Arg("<name>", Arg::text);
}

//----------------------------------------------------------------------------------------------------------------------

static StartCmd startCmd;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
