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

#include "eclib/Colour.h"
#include "eclib/DHSBehavior.h"
#include "eclib/DHSStdLogger.h"
#include "eclib/Log.h"
#include "eclib/Monitor.h"
#include "eclib/Resource.h"
#include "eclib/RotLogger.h"

DHSBehavior::DHSBehavior()
{
    try 
    {
        Monitor::startup();
	}
	catch(exception& e) {
		std::cerr << "** " << e.what() << " Caught in " << Here() <<  std::endl;
		std::cerr << "** Exception is re-thrown" << std::endl;
		throw;
	}
}

DHSBehavior::~DHSBehavior()
{
    Monitor::shutdown();    
}

string DHSBehavior::home() const
{
    static const char * dhshome = getenv( "DHSHOME" );
    if( !dhshome )
        throw SeriousBug("DHSHOME is not defined");
    return string( dhshome );
}


long DHSBehavior::taskId() const
{
    return Monitor::self();
}

bool DHSBehavior::toFile()
{
    return Resource<bool>("$DHS_FILE_LOGGING",false);
}

Logger* DHSBehavior::createInfoLogger()
{
    DHSLogger * logger = 0;
    if( ! toFile() )    
        logger = new DHSStdLogger( std::cout );
    else
        logger = new RotLogger(); 
    
    logger->prefix("(I) ");
    
    return logger;
}

Logger* DHSBehavior::createDebugLogger()
{    
    DHSLogger * logger = 0;
    if( ! toFile() )  
        logger = new DHSStdLogger( std::cout );
    else
        logger = new RotLogger();        
    
    logger->prefix("(D) ");
    
    return logger;
}

Logger* DHSBehavior::createWarningLogger()
{    
    DHSLogger * logger = 0;
    if( ! toFile() )    
        logger = new DHSStdLogger( std::cerr );
    else
        logger = new RotLogger();        
    
    logger->prefix("(W) ");
    logger->setColor(&Colour::yellow);
    logger->resetColor(&Colour::reset);
    
    return logger;
}

Logger* DHSBehavior::createErrorLogger()
{    
    DHSLogger * logger = 0;
    if( ! toFile() )    
        logger = new DHSStdLogger( std::cerr );
    else
        logger = new RotLogger();        
    
    logger->prefix("(E) ");
    logger->setColor(&Colour::red);
    logger->resetColor(&Colour::reset);
    
    return logger;
}


