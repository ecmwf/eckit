/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/FileHandle.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_filesystem_FileHandle_h
#define eckit_filesystem_FileHandle_h

#include "eckit/io/DataHandle.h"
#include "eckit/io/Buffer.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class FileHandle : public DataHandle {
public:

// -- Contructors

	FileHandle(const std::string&,bool = false);
	FileHandle(Stream&);

// -- Destructor

	~FileHandle();

// --  Methods

	void advance(const Length&); 
	const std::string& path() const { return name_; }

// -- Overridden methods

	// From DataHandle

	virtual Length openForRead();
	virtual void   openForWrite(const Length&);
	virtual void   openForAppend(const Length&);

	virtual long   read(void*,long);
	virtual long   write(const void*,long);
    virtual void   close();
    virtual void   flush();
	virtual void   rewind();
	virtual void   print(std::ostream&) const;
	virtual Length estimate();
	virtual Length saveInto(DataHandle&,TransferWatcher& = TransferWatcher::dummy());
	virtual Offset position();
	virtual bool isEmpty() const;
	virtual void restartReadFrom(const Offset& from);
	virtual void restartWriteFrom(const Offset& from);
    virtual void toRemote(Stream&) const;
    virtual void cost(std::map<std::string,Length>&, bool) const;
    virtual std::string title() const;
    virtual bool moveable() const { return true; }

    virtual Offset seek(const Offset&);
    virtual void skip(const Length&);

    virtual DataHandle* clone() const;

	// From Streamable

	virtual void encode(Stream&) const;
	virtual const ReanimatorBase& reanimator() const { return reanimator_; }

// -- Class methods

	static  const ClassSpec&  classSpec()        { return classSpec_;}

private:

// -- Members

	std::string              name_;
	bool                overwrite_;
	FILE*               file_;
	bool                read_;
    std::auto_ptr<Buffer>    buffer_;

// -- Methods

	void open(const char*);

// -- Class members

    static  ClassSpec               classSpec_;
	static  Reanimator<FileHandle>  reanimator_;

};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
