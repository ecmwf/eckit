/*
 * (C) Copyright 1996-2017 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Partio/FileHandle.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_filesystem_PartFileHandle_h
#define eckit_filesystem_PartFileHandle_h

#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/types/Types.h"
#include "eckit/memory/ScopedPtr.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class PartFileHandle : public DataHandle {
public:

// -- Contructors

	PartFileHandle(const PathName&,const OffsetList&,const LengthList&);
	PartFileHandle(const PathName&,const Offset&,const Length&);
	PartFileHandle(Stream&);

// -- Destructor

	~PartFileHandle();

// -- Methods

	const PathName& path() const { return name_; }

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
    virtual void toRemote(Stream&) const;
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

	PathName           name_;
#ifdef USE_LINKS
	PathName           link_;
#endif
	FILE*              file_;
	long long          pos_;
	Ordinal            index_;
	OffsetList         offset_;
	LengthList         length_;
	ScopedPtr<Buffer>  buffer_;

// -- Methods

	long read1(char*,long);

// -- Class members

    static  ClassSpec               classSpec_;
	static  Reanimator<PartFileHandle>  reanimator_;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
