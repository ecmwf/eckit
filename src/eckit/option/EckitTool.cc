// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
