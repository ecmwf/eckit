/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_ResourceFromFiles_h
#define eckit_ResourceFromFiles_h

#include <set>

#include "eckit/config/Script.h"
#include "eckit/memory/ScopedPtr.h"
#include "eckit/filesystem/PathName.h"

namespace eckit {

//-----------------------------------------------------------------------------

class LocalPathName;

class ResourceFromFiles {

public: // methods

  ResourceFromFiles();

  friend void configure( ResourceFromFiles& p, config::Script& script );

protected:

  /// @brief Append configuration file to the script
  void appendConfig( const PathName& path, config::Script& script );

  void readConfigFiles( config::Script& script );

};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
