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

#include "eckit/exception/Exceptions.h"
#include "eckit/option/CmdArgs.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static EckitTool* INSTANCE = nullptr;

static void usage(const std::string& tool) {
    ASSERT(INSTANCE != nullptr);
    INSTANCE->usage(tool);
}

EckitTool::EckitTool(int argc, char** argv) : Tool(argc, argv, "ECKIT_HOME") {
    ASSERT(INSTANCE == nullptr);
    INSTANCE = this;
}

void EckitTool::run() {
    option::CmdArgs args(&eckit::usage, options_, numberOfPositionalArguments(), minimumPositionalArguments());

    init(args);
    execute(args);
    finish(args);
}

void EckitTool::usage(const std::string& tool) const {}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
