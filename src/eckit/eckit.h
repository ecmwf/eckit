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

#endif

