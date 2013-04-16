/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <stdlib.h>

#include "eckit/runtime/Context.h"
#include "eclib/Loader.h"
#include "eclib/Log.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/config/Resource.h"

#include "eckit/runtime/Tool.h"
#include "eclib/runtime/ToolBehavior.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

Tool::Tool(int argc,char **argv) : 
    name_("undefined")
{
	name_ = PathName(argv[0]).baseName(false);
    
    Context::instance().setup( argc, argv, new ToolBehavior() );

    Context::instance().runName( name_ );
    
    Context::instance().behavior()->initialize();

    Loader::callAll(&Loader::execute);
}

Tool::~Tool()
{
}

void Tool::reconfigure()
{
    Log::info() << "Tool::reconfigure" << endl;
    
    int debug = Resource<int>(this,"debug;$DEBUG;-debug",0);
    
    Context::instance().debug( debug );

    // forward to context
    Context::instance().reconfigure();     
}

void Tool::start()
{
	int status = 0;

    int debug = Resource<int>(this,"debug;$DEBUG;-debug",0);
    
    Context::instance().debug( debug );

    string displayName = Resource<string>("-name",name_);
    
    Context::instance().displayName( displayName );
    
	try 
    {
        run();
	}
	catch(exception& e){
		status = 1;
		Log::error() << "** " << e.what() << " Caught in "  << Here() <<  endl;
		Log::error() << "** Exception is terminates " << name() << endl;
	}

	::exit(status);
}

//-----------------------------------------------------------------------------

} // namespace eckit

