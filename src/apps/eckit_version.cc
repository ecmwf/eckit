/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/eckit_version.h"

#include "eckit/runtime/Tool.h"
#include "eckit/config/Resource.h"

using namespace eckit;

class Version : public Tool {
public:

    Version(int argc,char **argv): Tool(argc,argv) {}

    ~Version() {}

    virtual void run();
};

//-----------------------------------------------------------------------------

void Version::run()
{
    if( Resource<bool>("-long",false) )
    {
        Log::info() << "eckit" << std::endl
                    << "    version: " << get_eckit_version() << std::endl
                    << "    version str: " << get_eckit_version_str() << std::endl
                    << "    sha1: " << get_eckit_git_sha1() << std::endl;

        if( Resource<bool>("-build",false) )
        {
            Log::info() << "    build type  : " << EC_BUILD_TYPE << std::endl
                        << "    timestamp   : " << EC_BUILD_TIMESTAMP << std::endl
                        << "    op. system  : " << EC_OS_NAME << " (" << EC_OS_STR << ")"  << std::endl
                        << "    processor   : " << EC_SYS_PROCESSOR  << std::endl
                        << "    c compiler  : " << EC_C_COMPILER_ID << " " << EC_C_COMPILER_VERSION << std::endl
                        << "      flags     : " << EC_C_FLAGS << std::endl
                        << "    c++ compiler: " << EC_CXX_COMPILER_ID << " " << EC_CXX_COMPILER_VERSION << std::endl
                        << "      flags     : " << EC_CXX_FLAGS << std::endl;
        }
    }
    else
    {
        Log::info() << get_eckit_version() << std::endl;
    }    
    
}

//-----------------------------------------------------------------------------

int main(int argc,char **argv)
{
    Version app(argc,argv);
    app.start();
    return 0;
}

