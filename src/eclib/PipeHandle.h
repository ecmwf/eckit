/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File PipeHandle.h
// Baudouin Raoult - ECMWF May 96

#ifndef PipeHandle_H
#define PipeHandle_H


#include "eclib/DataHandle.h"

class PipeHandle : public DataHandle {
public:

// -- Contructors

	PipeHandle(const string&);
	PipeHandle(Stream&);

// -- Destructor

	~PipeHandle();

// --  Methods

	void advance(const Length&); 

// -- Overridden methods

	// From DataHandle

	virtual Length openForRead();
	virtual void   openForWrite(const Length&);
	virtual void   openForAppend(const Length&);

	virtual long   read(void*,long);
	virtual long   write(const void*,long);
	virtual void   close();
	virtual void   rewind();
	virtual void   print(ostream&) const;
    /*
	virtual void restartReadFrom(const Offset& from);
	virtual void restartWriteFrom(const Offset& from);
    */
    /* virtual void toRemote(Stream&) const; */
    /* virtual void cost(map<string,Length>&, bool) const; */
    /* virtual string title() const; */
    virtual bool moveable() const { return false; }

    virtual Offset seek(const Offset&);

	// From Streamable

	virtual void encode(Stream&) const;
	virtual const ReanimatorBase& reanimator() const { return reanimator_; }

// -- Class methods

	static  const ClassSpec&  classSpec()        { return classSpec_;}

private:

// No copy allowed

	PipeHandle(const PipeHandle&);
	PipeHandle& operator=(const PipeHandle&);

// -- Members

	string        name_;
	FILE*         file_;
	bool          read_;

// -- Methods

	void open(const char*);

// -- Class members

    static  ClassSpec               classSpec_;
	static  Reanimator<PipeHandle>  reanimator_;

};

#endif
