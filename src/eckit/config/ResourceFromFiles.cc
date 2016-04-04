/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/thread/Once.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/config/ResourceFromFiles.h"
#include "eckit/config/FileReadPolicy.h"
#include "eckit/runtime/Context.h"
#include "eckit/runtime/ContextBehavior.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

static Once<Mutex> local_mutex;

void configure(ResourceFromFiles& p, config::Script& script)
{
  p.readConfigFiles(script);
}

ResourceFromFiles::ResourceFromFiles()
{
}

void ResourceFromFiles::appendConfig( const PathName& path, config::Script& script )
{
  FileReadPolicy p = Context::instance().behavior().fileReadPolicy();

  std::stringstream s;

  if( read( p, path, s ) )
    script.readStream(s);
}

void ResourceFromFiles::readConfigFiles(config::Script& script)
{
    AutoLock<Mutex> lock(local_mutex);

    std::string appName = Context::instance().runName();

    PathName general ("~/etc/config/general");
    PathName local ("~/etc/config/local");
    PathName app ( std::string("~/etc/config/" ) + appName );
    PathName applocal ( std::string("~/etc/config/" ) + appName + ".local" );

    appendConfig( general,  script );
    appendConfig( local,    script );
    appendConfig( app,      script );
    appendConfig( applocal, script );
}

//-----------------------------------------------------------------------------

} // namespace eckit

