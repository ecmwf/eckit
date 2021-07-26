/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "EckitTool.h"

#include "eckit/log/Log.h"
#include "eckit/option/CmdArgs.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static EckitTool* instance_ = 0;

EckitTool::EckitTool(int argc, char** argv) :
    eckit::Tool(argc, argv, "ECKIT_HOME") {
    ASSERT(instance_ == 0);
    instance_ = this;
}

static void usage(const std::string& tool) {
    ASSERT(instance_);
    instance_->usage(tool);
}

void EckitTool::run() {

    eckit::option::CmdArgs args(&eckit::usage,
                                options_,
                                numberOfPositionalArguments(),
                                minimumPositionalArguments());

    init(args);
    execute(args);
    finish(args);
}

void EckitTool::usage(const std::string& tool) const {
}

void EckitTool::init(const eckit::option::CmdArgs& args) {
}

void EckitTool::finish(const eckit::option::CmdArgs& args) {
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
