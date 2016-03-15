/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/FTPHandle.h
// Manuel Fuentes - ECMWF May 96

#ifndef eckit_filesystem_FTPHandle_h
#define eckit_filesystem_FTPHandle_h

#include "eckit/io/DataHandle.h"
#include "eckit/net/TCPClient.h"
#include "eckit/net/TCPSocket.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class FTPHandle : public DataHandle {
public:

// -- Exceptions

    class FTPError : public std::exception { virtual const char *what() const throw(); };

// -- Contructors

	FTPHandle(const std::string&,const std::string&,int port = 21);
	FTPHandle(Stream&);

// -- Destructor

	~FTPHandle() {}

// -- Overridden methods

	// From DataHandle

    virtual Length openForRead();
    virtual void openForWrite(const Length&);
    virtual void openForAppend(const Length&);

	virtual long read(void*,long);
	virtual long write(const void*,long);
	virtual void close();
	virtual void rewind();
	virtual void print(std::ostream&) const;

	// From Streamable

	virtual void encode(Stream&) const;
	virtual const ReanimatorBase& reanimator() const { return reanimator_; }

// -- Class methods

	static  const ClassSpec&  classSpec()        { return classSpec_;}

private:

// -- Members

	std::string        remote_;
	std::string        host_;
	int           port_;
	TCPClient     cmds_;
	TCPSocket     data_;

// -- Methods
	
	void   ftpCommand(const std::string&);
	std::string readLine();
	void   open(const std::string&);

// -- Class members

    static  ClassSpec               classSpec_;
	static  Reanimator<FTPHandle>  reanimator_;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
