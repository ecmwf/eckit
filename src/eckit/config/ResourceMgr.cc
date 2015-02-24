/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/thread/AutoLock.h"
#include "eckit/config/Configurable.h"
#include "eckit/runtime/Context.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/log/Log.h"
#include "eckit/thread/Mutex.h"
#include "eckit/config/ResourceMgr.h"
#include "eckit/types/Types.h"

#include "eckit/config/Compiler.h"
#include "eckit/config/Script.h"
#include "eckit/runtime/ContextBehavior.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

static Mutex local_mutex;
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
    AutoLock<Mutex> lock(local_mutex);

    inited_ = false;
    parsed_.clear();
    script_->clear();
}

void ResourceMgr::set(const std::string& name,const std::string& value)
{
    AutoLock<Mutex> lock(local_mutex);

    std::ostringstream code;
    code << name << " = " << value << std::endl;

    std::istringstream in( code.str() );

    script_->readStream(in);
}

void ResourceMgr::appendConfig(std::istream &in)
{
    AutoLock<Mutex> lock(local_mutex);

    script_->readStream(in);
}

bool ResourceMgr::appendConfig(const PathName& path)
{
    AutoLock<Mutex> lock(local_mutex);

    bool retval(false);
    if( parsed_.find(path) == parsed_.end() )
    {
       std::string config = Context::instance().behavior().readConfig(path);
       std::stringstream stream; stream << config;
       script_->readStream( stream );
       parsed_.insert(path);
       retval = (config.size() > 0);
    }
    return retval;
}

void ResourceMgr::readConfigFiles()
{
    AutoLock<Mutex> lock(local_mutex);

    if(!inited_)
    {
        inited_ = true;

        std::string appName = Context::instance().runName();

        PathName general ("~/etc/config/general");
        PathName local ("~/etc/config/local");
        PathName app ( std::string("~/etc/config/" ) + appName );
        PathName applocal ( std::string("~/etc/config/" ) + appName + ".local" );

        appendConfig( general );
        appendConfig( local );
        appendConfig( app );
        appendConfig( applocal );
    }
}

bool ResourceMgr::lookUp( Configurable* owner,
                          const std::string& name,
                          const StringDict* args,
                          std::string& result)
{
    AutoLock<Mutex> lock(local_mutex);

    readConfigFiles();

    StringDict resmap;

    if(args)
        script_->execute( *args, resmap );
    else
    {
        script_->execute( StringDict(), resmap );
    }

#if 0 // DEBUG
    Log::error() << "name [" << name << "] looking in " << (args?"args ":"StringDict()") << std::endl;
    if(args)  { Log::error() << "args [" ; __print_container(Log::error(),*args); Log::error() << "]" << std::endl; }
    Log::error() << "resmap [" ; __print_container(Log::error(),resmap); Log::error() << "]" << std::endl;
    script_->print( Log::error() );
#endif

    StringDict::iterator i;

    if( owner )
    {
        std::string kind  = owner->kind();
        std::string owner_name = owner->name();

        i = resmap.find( std::string( kind + "." + owner_name + "." + name ) );

        if(i != resmap.end())
        {
            result = (*i).second;
#if 0 // DEBUG
    Log::error() << "result : "<< result << " " << Here() << std::endl;
#endif
            return true;
        }

        i = resmap.find( std::string( owner_name + "." + name ) );

        if(i != resmap.end())
        {
            result = (*i).second;
#if 0 // DEBUG
    Log::error() << "result : "<< result << " " << Here() << std::endl;
#endif
            return true;
        }
    }

    i = resmap.find( name );

    if( i != resmap.end() )
    {
        result = (*i).second;
#if 0 // DEBUG
    Log::error() << "result : "<< result << " " << Here() << std::endl;
#endif
        return true;
    }

#if 0 // DEBUG
    Log::error() << "result not found " << Here() << std::endl;
#endif
    return false;
}

//-----------------------------------------------------------------------------

} // namespace eckit

