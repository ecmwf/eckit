/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eclib/Application.h"
#include "eclib/AutoLock.h"
#include "eclib/Configurable.h"
#include "eclib/LocalPathName.h"
#include "eclib/Log.h"
#include "eclib/Mutex.h"
#include "eclib/ResourceMgr.h"

#include "eclib/config/Compiler.h"
#include "eclib/config/Script.h"

static Mutex mutex;

static config::Script script;

bool ResourceMgr::inited_ = false;

void ResourceMgr::reset()
{
	AutoLock<Mutex> lock(mutex);
    
    inited_ = false;
}

void ResourceMgr::set(const string& name,const string& value)
{
	AutoLock<Mutex> lock(mutex);

    ostringstream code;
    code << name << " = " << value << std::endl;
    
    istringstream in( code.str() );
    
    script.readStream(in);
}

void ResourceMgr::appendConfig(istream &in)
{
    AutoLock<Mutex> lock(mutex);

    script.readStream(in);
}

void ResourceMgr::readConfigFiles()
{
    AutoLock<Mutex> lock(mutex);

    if(!inited_)
	{
		inited_ = true;
        
        script.readFile( "~/etc/config/general" );
        script.readFile( "~/etc/config/local" );
        script.readFile( string("~/etc/config/" ) + Application::appName() );
        script.readFile( string("~/etc/config/" ) + Application::appName() + ".local" );
	}
}

bool ResourceMgr::lookUp( Configurable* owner,
                          const string& name, 
                          const StringDict* args, 
                          string& result)
{
    AutoLock<Mutex> lock(mutex);
    
    readConfigFiles();
    
    StringDict resmap;

    if(args)
        script.execute( *args, resmap );
    else
    {
        script.execute( StringDict(), resmap );
    }

#if 0 // DEBUG
    std::cerr << "name [" << name << "]" << std::endl;
    std::cerr << "resmap [" << resmap << "]" << std::endl;
    script.print( std::cerr );
#endif

	StringDict::iterator i;
    
    if( owner )
    {
        string kind  = owner->kind();
        string owner_name = owner->name();

        i = resmap.find( string( kind + "." + owner_name + "." + name ) );

        if(i != resmap.end())
        {
            result = (*i).second;
            return true;
        }

        i = resmap.find( string( owner_name + "." + name ) );

        if(i != resmap.end())
        {
            result = (*i).second;
            return true;
        }
    }

    i = resmap.find( string( name ) );

    if( i != resmap.end() )
	{
		result = (*i).second;
		return true;
	}

    return false;
}


