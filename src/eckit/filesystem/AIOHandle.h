/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_AIOHandle_h
#define eckit_AIOHandle_h

#include <aio.h>

#include "eclib/machine.h"

#include "eclib/Buffer.h"
#include "eclib/DataHandle.h"
#include "eckit/filesystem/PathName.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class AIOHandle : public DataHandle {

public: // methods


    /// Contructor

	AIOHandle(const PathName& path,size_t count = 64, size_t = 1024*1024,bool fsync = false);

    /// Destructor

	virtual ~AIOHandle();

    // From DataHandle

    virtual Length openForRead();
    virtual void openForWrite(const Length&);
    virtual void openForAppend(const Length&);

    virtual long read(void*,long);
    virtual long write(const void*,long);
    virtual void close();
    virtual void flush();
    virtual void rewind();
    virtual void print(ostream&) const;

	virtual Length estimate();
	virtual Offset position();

    // From Streamable

#if 0
    virtual void encode(Stream&) const;
    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
#endif

// -- Class methods

#if 0
    static  const ClassSpec&  classSpec()        { return classSpec_;}
#endif

private: // members

	PathName                path_;
	vector<Buffer*>         buffers_;
	vector<const aiocb64 *> aiop_;
	vector<aiocb64>         aio_;
	vector<long>			len_;
	
	size_t                  used_;
	size_t                  count_;
	
	int                     fd_;
	off64_t                 pos_;
	bool                    fsync_;
	

    virtual string title() const;

// -- Class members

#if 0
	static  ClassSpec                 classSpec_;
    static  Reanimator<AIOHandle>  reanimator_;
#endif

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
