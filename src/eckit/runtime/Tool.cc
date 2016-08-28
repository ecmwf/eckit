/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// #include <stdlib.h>

#include "eckit/runtime/Tool.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Tool::Tool(int argc, char **argv, const char* homeenv) :
    Main(argc, argv, homeenv)
{
}

Tool::~Tool()
{
}

int Tool::start()
{
    int status = 0;

    // int debug = Resource<int>(this, "debug;$DEBUG;-debug", 0);

    // Main::instance().debugLevel( debug );


    try
    {
        run();
    }
    catch ( Exception& e ) {
        status = 1;
        Log::error() << "** " << e.what() << " Caught in "  << Here() << std::endl;
        Log::error() << "** Exception terminates " << name() << std::endl;
    }
    catch ( std::exception& e ) {
        status = 1;
        Log::error() << "** " << e.what() << " Caught in "  << Here() << std::endl;
        Log::error() << "** Exception terminates " << name() << std::endl;
    }

    return status;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

