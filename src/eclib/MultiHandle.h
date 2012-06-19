/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File MultiHandle.h
// Manuel Fuentes - ECMWF Jul 96

#ifndef MultiHandle_H
#define MultiHandle_H

#include "eclib/DataHandle.h"

class MultiHandle : public DataHandle {
public:

	typedef vector<DataHandle*> HandleList;

// -- Contructors

	MultiHandle();
	MultiHandle(const LengthList&);
	MultiHandle(const HandleList&);
	MultiHandle(const HandleList&,const LengthList&);
	MultiHandle(Stream&);

// -- Destructor

	~MultiHandle();

// -- Operators

	virtual void operator+=(DataHandle*);
	virtual void operator+=(const Length&);

// -- Overridden methods

    // From DataHandle

    virtual Length openForRead();
    virtual void openForWrite(const Length&);
    virtual void openForAppend(const Length&);

    virtual long read(void*,long);
    virtual long write(const void*,long);
    virtual void close();
    virtual void rewind();
    virtual void print(ostream&) const;
	void restartReadFrom(const Offset& from);


	virtual bool merge(DataHandle*);
	virtual bool compress(bool = false);

	virtual Length estimate();
    virtual void toRemote(Stream&) const;
    virtual void toLocal(Stream&) const;
    virtual DataHandle* toLocal();
    virtual void cost(map<string,Length>&, bool) const;
    virtual string title() const;
    virtual bool moveable() const;

    // From Streamable

    virtual void encode(Stream&) const;
    virtual const ReanimatorBase& reanimator() const { return reanimator_; }

// -- Class methods

    static  const ClassSpec&  classSpec()        { return classSpec_;}

private:

// -- Members

	HandleList			    datahandles_;
	HandleList::iterator 	current_;
	LengthList::iterator    curlen_;
	LengthList              length_;
	Length                  written_;
	bool                    read_;

// -- Methods

	void openCurrent();
	void open();
	long read1(char*,long);

// -- Class members

	static  ClassSpec                 classSpec_;
    static  Reanimator<MultiHandle>  reanimator_;

};


#endif
