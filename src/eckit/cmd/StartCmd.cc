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

void StartCmd::execute(std::istream&, std::ostream& out, CmdArg& args) {

    std::string app = args[1];
    std::set<std::string> names;

    bool wait   = args.exists("wait");
    int timeout = 120;

    if (wait) {
        timeout = int(args["wait"]);
    }

    if (app == "all") {

        PathName path("~/etc/applications");

        if (path.exists()) {
            std::ifstream in(path.localPath());
            std::string line;
            while (in >> line) {
                names.insert(line);
            }
        }
        else {
            std::string all = Resource<std::string>("allApplications", "mars,reader,flusher,cleaner,httpsvr,safety");

            std::vector<std::string> tmp;
            Tokenizer token(", \t");
            token(all, tmp);

            names = std::set<std::string>(tmp.begin(), tmp.end());
        }
    }
    else {
        names.insert(app);
    }

    Monitor::TaskArray& info = Monitor::instance().tasks();

    for (size_t w = 0; w < timeout; w++) {
        for (auto i = names.begin(); i != names.end(); ++i) {
            start(out, *i);
        }

        if (wait) {
            ::sleep(1);
        }

        for (size_t j = 0; j < info.size(); j++) {
            if (info[j].busy(true)) {
                if (names.find(info[j].application()) != names.end()) {
                    names.erase(info[j].application());
                }
            }
        }

        if (!wait || names.size() == 0) {
            break;
        }
    }
    if (wait && names.size() > 0) {
        std::ostringstream oss;
        oss << "Could not start task(s): " << names;
        throw SeriousBug(oss.str());
    }
}

//----------------------------------------------------------------------------------------------------------------------

void StartCmd::start(std::ostream& out, const std::string& app) const {
    Monitor::instance().start(app);
}

//----------------------------------------------------------------------------------------------------------------------

void StartCmd::help(std::ostream&) const {}

//----------------------------------------------------------------------------------------------------------------------

Arg StartCmd::usage(const std::string& cmd) const {
    return ~Arg("-wait", Arg::number) + (Arg("all") | Arg("<name>", Arg::text));
}

//----------------------------------------------------------------------------------------------------------------------

static StartCmd startCmd;

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
