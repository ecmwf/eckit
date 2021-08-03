/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <memory>
#include <string>

#include "eckit/filesystem/URI.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/cluster/ClusterNodes.h"
#include "eckit/log/Log.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/runtime/Tool.h"

using eckit::DataHandle;
using eckit::Log;
using eckit::URI;

class DHCopy : public eckit::Tool {
public:
    DHCopy(int argc, char** argv) :
        Tool(argc, argv, "DHSHOME") {
        options_.push_back(new eckit::option::SimpleOption<std::string>("from", "copy from this URI"));
        options_.push_back(new eckit::option::SimpleOption<std::string>("to", "copy to this URI"));
        options_.push_back(new eckit::option::SimpleOption<bool>("compare", "compare output"));
    }

    virtual void run();

    std::vector<eckit::option::Option*> options_;
};

static void usage(const std::string& tool) {

    Log::info() << "Usage: " << tool << " -from [URI1] -to [URI2]" << std::endl
                << std::endl;
}


void DHCopy::run() {

    eckit::ClusterNodes::list(Log::info());

    eckit::option::CmdArgs args(&usage, options_, 0, 0);

    URI from(args.getString("from"));
    URI to(args.getString("to"));

    std::unique_ptr<DataHandle> r(from.newReadHandle());
    std::unique_ptr<DataHandle> w(to.newWriteHandle());

    r->saveInto(*w);

    if (args.getBool("compare", false)) {
        std::unique_ptr<DataHandle> c(to.newReadHandle());
        ASSERT(r->compare(*c));
    }
}


//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    DHCopy tool(argc, argv);
    return tool.start();
}
