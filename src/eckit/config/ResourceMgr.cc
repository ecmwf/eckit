/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/thread/AutoLock.h"
#include "eclib/config/Configurable.h"
#include "eclib/Context.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eclib/Log.h"
#include "eckit/thread/Mutex.h"
#include "eckit/config/ResourceMgr.h"
#include "eclib/Types.h"

#include "eckit/config/Compiler.h"
#include "eclib/config/Script.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

static Mutex mutex;
static Mutex mutex_instance;

ResourceMgr::ResourceMgr() :
    inited_(false),
    script_( new config::Script() ),
    parsed_()
{
}

ResourceMgr::~ResourceMgr()
{
    delete script_;
}

ResourceMgr& ResourceMgr::instance()
{
    AutoLock<Mutex> lock(mutex_instance);
    
    static ResourceMgr* obj = 0;
    
    if( !obj )
        obj = new ResourceMgr();
    
    return *obj;
}

void ResourceMgr::reset()
{
	AutoLock<Mutex> lock(mutex);
    
    inited_ = false;
    parsed_.clear();
    script_->clear();
}

void ResourceMgr::set(const string& name,const string& value)
{
	AutoLock<Mutex> lock(mutex);

    ostringstream code;
    code << name << " = " << value << std::endl;
    
    istringstream in( code.str() );
    
    script_->readStream(in);
}

void ResourceMgr::appendConfig(istream &in)
{
    AutoLock<Mutex> lock(mutex);

    script_->readStream(in);
}

void ResourceMgr::appendConfig(const PathName& path)
{
    AutoLock<Mutex> lock(mutex);
    
    if( parsed_.find(path) == parsed_.end() )
    {
        script_->readFile(path);
        parsed_.insert(path);
    }
}

void ResourceMgr::readConfigFiles()
{
    AutoLock<Mutex> lock(mutex);

    if(!inited_)
	{
		inited_ = true;
        
        std::string appName = Context::instance().runName();
        
        PathName general ("~/etc/config/general");
        PathName local ("~/etc/config/local");
        PathName app ( string("~/etc/config/" ) + appName );
        PathName applocal ( string("~/etc/config/" ) + appName + ".local" );
        
        if( script_->readFile( general  ) ) parsed_.insert(general);
        if( script_->readFile( local    ) ) parsed_.insert(local);
        if( script_->readFile( app      ) ) parsed_.insert(app);
        if( script_->readFile( applocal ) ) parsed_.insert(applocal);
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
        script_->execute( *args, resmap );
    else
    {
        script_->execute( StringDict(), resmap );
    }

#if 0 // DEBUG
    std::cerr << "name [" << name << "]" << std::endl;
    if(args)  { std::cerr << "args [" ; __print_container(std::cerr,*args); std::cerr << "]" << std::endl; }
    std::cerr << "resmap [" ; __print_container(std::cerr,resmap); std::cerr << "]" << std::endl;
    script_->print( std::cerr );
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

    i = resmap.find( name );

    if( i != resmap.end() )
	{
		result = (*i).second;
		return true;
	}

    return false;
}

//-----------------------------------------------------------------------------

} // namespace eckit

