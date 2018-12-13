/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_eckit_h
#define eckit_eckit_h

#include "eckit/eckit_config.h"

//-----------------------------------------------------------------------------

/* POSIX */

#include <cstring>  // for memset

//-----------------------------------------------------------------------------

/* STL */

// #include <set>
// #include <map>
// #include <list>
// #include <vector>
// #include <stack>
// #include <queue>
// #include <algorithm>
// #include <memory>
// #include <numeric>

// #include <iostream>
// #include <iomanip>
// #include <fstream>
// #include <new>
// #include <sstream>

#include <iterator>  // for std::output_iterator_tag
//#include <iosfwd>

//--------------------------------------------------------------------------------------------------

#if (!defined ECKIT_HAVE_MAP_ANONYMOUS) && (defined ECKIT_HAVE_MAP_ANON)
#define MAP_ANONYMOUS MAP_ANON
#endif

#if (!defined ECKIT_HAVE_FDATASYNC) && (defined ECKIT_HAVE_FSYNC)
#define fdatasync  fsync
#endif

/* Usefull macros */

#ifndef NUMBER
#define NUMBER(x) (sizeof(x)/sizeof(x[0]))
#endif

//--------------------------------------------------------------------------------------------------

namespace eckit {

struct output_iterator {
  typedef std::output_iterator_tag iterator_category;
  typedef void                value_type;
  typedef void                difference_type;
  typedef void                pointer;
  typedef void                reference;
};

template<class T>
inline void zero(T& p) { ::memset(&p,0,sizeof(T)); }

} // namespace eckit

//--------------------------------------------------------------------------------------------------

#endif

