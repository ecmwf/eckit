/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/eckit.h"

#include "eckit/config/Resource.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

class Version : public Tool {
public:
    Version(int argc, char** argv) :
        Tool(argc, argv) {}

    ~Version() {}

    virtual void run();
};

void Version::run() {
    if (Resource<bool>("-long", false)) {
        Log::info() << "eckit" << std::endl
                    << "    version: " << eckit_version() << std::endl
                    << "    version str: " << eckit_version_str() << std::endl
                    << "    sha1: " << eckit_git_sha1() << std::endl;

        if (Resource<bool>("-build", false)) {
            Log::info() << "    build type  : " << ECKIT_BUILD_TYPE << std::endl
                        << "    timestamp   : " << ECKIT_BUILD_TIMESTAMP << std::endl
                        << "    op. system  : " << ECKIT_OS_NAME << " (" << ECKIT_OS_STR << ")" << std::endl
                        << "    processor   : " << ECKIT_SYS_PROCESSOR << std::endl
                        << "    c compiler  : " << ECKIT_C_COMPILER_ID << " " << ECKIT_C_COMPILER_VERSION << std::endl
                        << "      flags     : " << ECKIT_C_FLAGS << std::endl
                        << "    c++ compiler: " << ECKIT_CXX_COMPILER_ID << " " << ECKIT_CXX_COMPILER_VERSION
                        << std::endl
                        << "      flags     : " << ECKIT_CXX_FLAGS << std::endl;
        }
    }
    else {
        Log::info() << eckit_version() << std::endl;
    }
}

}  // namespace eckit


//----------------------------------------------------------------------------------------------------------------------


int main(int argc, char** argv) {
    eckit::Version app(argc, argv);
    return app.start();
}
