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
#include <execinfo.h>  // for backtrace

#include "eclib/machine.h"

#ifdef EC_HAVE_CXXABI_H
  #include <cxxabi.h>
#endif

#include "eclib/BackTrace.h"
#include "eclib/Types.h"

////////////////////////////////////////////////////////////////////////////////

std::string BackTrace::dump()
{
    static Ordinal count = 0;
    ++count;
    
#define BUFFER_SIZE 256

    void*   buffer[BUFFER_SIZE];
    char**  strings;

    int addsize = backtrace(buffer, BUFFER_SIZE);

#undef BUFFER_SIZE
    
    std::ostringstream oss;
   
    oss << "backtrace [" << count << "] stack has " << addsize << " addresses\n";
    
    strings = backtrace_symbols(buffer, addsize);
    if (strings == NULL)
      oss << " --- no backtrace_symbols found ---\n";
    
#if 0 // EC_HAVE_CXXABI_H
    /// here we can parse the output of the dump and demangle the symbols using the cxxabi
#else
    for (size_t s = 0; s < addsize; ++s)
      oss << strings[s] << "\n";
#endif
    
    free(strings);
    
    oss << "\nexit dumping backtrace ...";
    
    return oss.str();
}
