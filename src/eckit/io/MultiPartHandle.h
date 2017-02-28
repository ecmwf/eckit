/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/MultiPartHandle.h
// Manuel Fuentes - ECMWF Jul 96

#ifndef eckit_filesystem_MultiPartHandle_h
#define eckit_filesystem_MultiPartHandle_h

#include "eckit/io/DataHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class MultiPartHandle : public DataHandle {
public:

// -- Contructors

	MultiPartHandle(DataHandle* handle, const Length& length, MultiPartHandle* prev);


// -- Destructor

	~MultiPartHandle();

// -- Overridden methods

    // From DataHandle

    virtual Length openForRead();
    virtual void openForWrite(const Length&);
    virtual void openForAppend(const Length&);

    virtual long read(void*,long);
    virtual long write(const void*,long);
    virtual void close();
    virtual void flush();
    virtual void rewind();
    virtual void print(std::ostream&) const;

	void restartReadFrom(const Offset& from);

    virtual Offset seek(const Offset&);
    virtual Offset position();
    virtual void skip(const Length &);

	virtual bool merge(DataHandle*);
	virtual bool compress(bool = false);

	virtual Length estimate();
    virtual void toRemote(Stream&) const;
    virtual void toLocal(Stream&) const;
    virtual DataHandle* toLocal();
    virtual void cost(std::map<std::string,Length>&, bool) const;
    virtual std::string title() const;
    virtual bool moveable() const;

    // From Streamable


// -- Class methods


private:

// -- Members

    DataHandle*      handle_;
    Length           size_;
    MultiPartHandle* prev_;
    MultiPartHandle* next_;
    Offset           position_;
    Offset           start_;
    bool             opened_;
    size_t           opens_;

// -- Methods

    MultiPartHandle& first();
    void openHandle();
    void closeHandle();

// -- Class members


};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
