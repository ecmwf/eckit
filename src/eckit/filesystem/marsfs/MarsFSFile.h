/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File MarsFSFile.h
// Baudouin Raoult - (c) ECMWF Jun 11

#ifndef eckit_MarsFSFile_h
#define eckit_MarsFSFile_h

#include "eckit/thread/AutoLock.h"
#include "eckit/filesystem/marsfs/MarsFSClient.h"
#include "eckit/filesystem/marsfs/MarsFSPath.h"
#include "eckit/net/TCPClient.h"
#include "eckit/memory/ScopedPtr.h"
#include "eckit/utils/Hash.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class MarsFSFile : private MarsFSClient {
public:

// -- Exceptions
	// None

// -- Contructors

	MarsFSFile(const MarsFSPath&);

// -- Destructor

	~MarsFSFile(); 

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods
//
    Length open(const char* mode, bool overwrite = false);
    long read(void*,long);
    long write(const void*,long);
    void close();
    void skip(const Length&);
    Offset seek(const Offset&);
    Length length();
	

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

protected:

// -- Members
	// None

// -- Methods
	
	void print(std::ostream&) const; 

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:

// No copy allowed

	MarsFSFile(const MarsFSFile&);
	MarsFSFile& operator=(const MarsFSFile&);

// -- Members
//
    TCPClient data_;
    MarsFSPath path_;
	AutoLock<Connector> lock_;

    eckit::ScopedPtr<Hash> hash_;

    bool doHash_;

// -- Methods
	// None

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	friend std::ostream& operator<<(std::ostream& s,const MarsFSFile& p)
		{ p.print(s); return s; }

};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
