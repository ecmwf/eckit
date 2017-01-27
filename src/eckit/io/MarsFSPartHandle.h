/*
 * (C) Copyright 1996-2017 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/MarsFSPartHandle.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_filesystem_MarsFSPartHandle_h
#define eckit_filesystem_MarsFSPartHandle_h

#include "eckit/io/DataHandle.h"
#include "eckit/filesystem/marsfs/MarsFSFile.h"
#include "eckit/filesystem/marsfs/MarsFSPath.h"
#include "eckit/memory/ScopedPtr.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class MarsFSPartHandle : public DataHandle {
public:

// -- Contructors

	MarsFSPartHandle(const MarsFSPath&,const OffsetList&,const LengthList&);
	MarsFSPartHandle(const MarsFSPath&,const Offset&,const Length&);
	MarsFSPartHandle(Stream&);

// -- Destructor

	~MarsFSPartHandle();

// -- Methods

	const MarsFSPath& path() const { return path_; }

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
	virtual bool merge(DataHandle*);
	virtual bool compress(bool = false);
	virtual Length estimate();
	virtual void restartReadFrom(const Offset& from);

    virtual DataHandle* toLocal();
    virtual void toLocal(Stream&) const;
    virtual void cost(std::map<std::string,Length>&, bool) const;
    virtual std::string title() const;
    virtual bool moveable() const { return true; }

	// From Streamable

	virtual void encode(Stream&) const;
	virtual const ReanimatorBase& reanimator() const { return reanimator_; }

// -- Class methods

	static  const ClassSpec&  classSpec()        { return classSpec_;}

private:

// -- Members

	MarsFSPath         path_;
	long long          pos_;
	Ordinal            index_;
	OffsetList         offset_;
	LengthList         length_;

	ScopedPtr<MarsFSFile>  file_;

// -- Methods

	long read1(char*,long);

// -- Class members

    static  ClassSpec               classSpec_;
	static  Reanimator<MarsFSPartHandle>  reanimator_;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
