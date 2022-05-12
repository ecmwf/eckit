/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/runtime/Tool.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/OStreamTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Tool::Tool(int argc, char** argv, const char* homeenv) :
    Main(argc, argv, homeenv), sendLogErrWarnToStdOut_(false) {}

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
