/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_os_Stat_h
#define eckit_os_Stat_h

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "eckit/eckit.h"

namespace eckit {

//-----------------------------------------------------------------------------

/// struct to manage differences between stat and stat64 different OS's
struct Stat
{
    /// prefer using stat if supports 64 bit

    #if ( EC_STAT_SIZEOF_OFF_T == 8) || (defined EC_HAVE_STAT) && (!defined EC_HAVE_STAT64)

    typedef struct stat Struct;

    static int stat (const char *path, Struct *s){ return ::stat(path,s); }
    static int lstat(const char *path, Struct *s){ return ::lstat(path,s); }
    static int fstat(int fd, Struct *s)          { return ::fstat(fd,s); }

    #else
        #if (defined EC_HAVE_STAT64)
    typedef Struct stat64;

    static int stat (const char *path, Struct *s){ return ::stat64(path,s);  }
    static int lstat(const char *path, Struct *s){ return ::lstat64(path,s); }
    static int fstat(int fd, Struct *s)          { return ::fstat64(fd,s);   }

        #else
            #error "could not define if to use stat or stat64"
        #endif
    #endif

private:

    Stat(); ///< non-instantiable

};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
