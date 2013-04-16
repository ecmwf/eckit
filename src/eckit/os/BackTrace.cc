/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>

#include "eckit/machine.h"

#ifdef EC_HAVE_EXECINFO_BACKTRACE
#include <execinfo.h>  // for backtrace
#endif

#ifdef EC_HAVE_CXXABI_H
  #include <cxxabi.h>
#endif

#include "eckit/os/BackTrace.h"
#include "eckit/types/Types.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

std::string BackTrace::dump()
{
    /// @todo implement this using the cxxabi demangle, if CMake detects it
    
    std::ostringstream oss;
    
#if defined( EC_HAVE_EXECINFO_BACKTRACE ) && !defined( AIX )

    static Ordinal count = 0;
    ++count;
    
#define BS_BUFF_SIZE 256

    void*   buffer[BS_BUFF_SIZE];
    char**  strings;

    int addsize = backtrace(buffer, BS_BUFF_SIZE);    
   
    oss << "backtrace [" << count << "] stack has " << addsize << " addresses\n";
    
    strings = backtrace_symbols(buffer, addsize);
    if (strings == NULL)
      oss << " --- no backtrace_symbols found ---\n";
    
/// @todo here we should parse the output of the dump and demangle the symbols using the cxxabi

#if 0 // EC_HAVE_CXXABI_H
#else
    for (int s = 0; s < addsize; ++s)
      oss << strings[s] << "\n";
#endif
    
    free(strings);
    
    oss << "\nexit dumping backtrace ...";
    
#else
    oss << "\ndumping backtrace not supported on this system";
#endif
    
    return oss.str(); 
}

//-----------------------------------------------------------------------------

} // namespace eckit

