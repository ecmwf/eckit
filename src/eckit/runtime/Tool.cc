// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/runtime/Tool.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/log/OStreamTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Tool::Tool(int argc, char** argv, const char* homeenv) : Main(argc, argv, homeenv), sendLogErrWarnToStdOut_(false) {}

Tool::~Tool() {}

int Tool::start() {
    int status = 0;

    try {
        run();
    }
    catch (Exception& e) {
        status = 1;
        Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
        Log::error() << "** Exception terminates " << name() << std::endl;
    }
    catch (std::exception& e) {
        status = 1;
        Log::error() << "** " << e.what() << " Caught in " << Here() << std::endl;
        Log::error() << "** Exception terminates " << name() << std::endl;
    }

    return status;
}

LogTarget* Tool::createWarningLogTarget() const {
    return new OStreamTarget(sendLogErrWarnToStdOut_ ? std::cout : std::cerr);
}

LogTarget* Tool::createErrorLogTarget() const {
    return new OStreamTarget(sendLogErrWarnToStdOut_ ? std::cout : std::cerr);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
