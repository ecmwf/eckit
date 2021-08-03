/*
 * (C) Copyright 2020- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <string>
#include <vector>

#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/runtime/Tool.h"
#include "eckit/utils/Rsync.h"

using eckit::Log;
using eckit::PathName;
using eckit::Rsync;

class RsyncTool : public eckit::Tool {
public:
    RsyncTool(int argc, char** argv) :
        Tool(argc, argv) {
        options_.push_back(new eckit::option::SimpleOption<std::string>("from", "copy from this path"));
        options_.push_back(new eckit::option::SimpleOption<std::string>("to", "copy to this path"));
        options_.push_back(new eckit::option::SimpleOption<bool>("recursive", "perform a recursive copy"));
        options_.push_back(new eckit::option::SimpleOption<bool>("statistics", "show per-file statistics"));
    }

    virtual void run();

    std::vector<eckit::option::Option*> options_;
};

static void usage(const std::string& tool) {

    Log::info() << "Usage: " << tool << " [--recursive] [--statistics] --from=[PATH1] --to=[PATH2]" << std::endl
                << std::endl;
}


void RsyncTool::run() {

    eckit::option::CmdArgs args(&usage, options_, 0, 0);

    bool recursive  = args.getBool("recursive", false);
    bool statistics = args.getBool("statistics", false);

    PathName from(args.getString("from"));
    PathName to(args.getString("to"));

    Rsync rsync(statistics);

    if (recursive)
        rsync.syncRecursive(from, to);
    else
        rsync.syncData(from, to);
}


//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    RsyncTool tool(argc, argv);
    return tool.start();
}
