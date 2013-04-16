/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#pragma once
#ifndef eckit_filesystem_FileSystemSize_h
#define eckit_filesystem_FileSystemSize_h

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

struct FileSystemSize {
	unsigned long long available;
	unsigned long long total;
	FileSystemSize() : available(0), total(0) {}
};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif // eckit_FileSystemSize_H
