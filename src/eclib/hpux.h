/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef mars_hpux_h
#define mars_hpux_h

/*
#define _HPUX_SOURCE 1
#define _LARGEFILE64_SOURCE 1
#define _XOPEN_SOURCE_EXTENDED 1
*/

#define strtoull __strtoull
#define strtoll __strtoll

/* C headers. */

#include <pthread.h>
#include <inttypes.h>
#include <sys/fcntl.h>
#include <sys/sched.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#endif /* mars_hpux_h */
