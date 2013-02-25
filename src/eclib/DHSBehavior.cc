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

#include "eclib/Application.h"
#include "eclib/Colour.h"
#include "eclib/DHSBehavior.h"
#include "eclib/DHSStdLogger.h"
#include "eclib/Log.h"
#include "eclib/Monitor.h"
#include "eclib/Resource.h"
#include "eclib/RotLogger.h"

//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

DHSBehavior::DHSBehavior() :
    taskId_(0),
    name_()
{
    char* home = getenv( "DHSHOME" );
    if( ! home )
    {
        /// @note can't use Log::error() here because it depends on this constructor

        std::cout << "WARNING: DHSHOME environment variable not defined -- trying to use HOME instead @ " << Here() <<  std::endl;
        home = getenv( "HOME" );
        if( ! home )
        {
            std::cerr << "ERROR: neither DHSHOME or HOME environment variables are defined -- cannot initialize DHSBehavior @ " << Here() <<  std::endl;
            ::exit(1);
        }
    }

    home_ = home;
}

DHSBehavior::~DHSBehavior()
{
    Monitor::shutdown();
}

void DHSBehavior::initialize()
{
    try
    {
        Monitor::active( true );
        Monitor::startup();
        taskId_ = Monitor::self();
    }
    catch(exception& e) {
        std::cerr << "** " << e.what() << " Caught in " << Here() <<  std::endl;
        std::cerr << "** Exception is re-thrown" << std::endl;
        throw;
    }
}

void DHSBehavior::finalize()
{
}

string DHSBehavior::runName() const
{
    if( name_.empty())
        return Application::appName();
    else 
        return name_;
}

void DHSBehavior::runName( const string& name )
{
    name_ = name;
}

long DHSBehavior::taskId() const
{
    return taskId_;
}

Logger* DHSBehavior::createInfoLogger()
{
    DHSLogger * logger = getLogger();
    logger->prefix("(I) ");
    return logger;
}

Logger* DHSBehavior::createDebugLogger()
{    
    DHSLogger * logger = getLogger();
    logger->prefix("(D) ");
    return logger;
}

Logger* DHSBehavior::createWarningLogger()
{    
    DHSLogger * logger = getLogger();
    logger->prefix("(W) ");
    logger->setColor(&Colour::yellow);
    logger->resetColor(&Colour::reset);
    return logger;
}

Logger* DHSBehavior::createErrorLogger()
{    
    DHSLogger * logger = getLogger();
    logger->prefix("(E) ");
    logger->setColor(&Colour::red);
    logger->resetColor(&Colour::reset);
    return logger;
}

//-----------------------------------------------------------------------------

} // namespace eclib

