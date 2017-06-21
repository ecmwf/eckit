/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/URLHandle.h
// Baudouin Raoult - ECMWF Jun 17

#ifndef eckit_filesystem_URLHandle_h
#define eckit_filesystem_URLHandle_h

#include "eckit/io/DataHandle.h"
#include "eckit/io/CircularBuffer.h"
#include "eckit/io/EasyCURL.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class URLHandle : public DataHandle, private EasyCURL {
public:


// -- Exceptions

// -- Contructors

    URLHandle(const std::string& uri);

    URLHandle(Stream&);

// -- Destructor

    ~URLHandle();

// -- Overridden methods

    // From DataHandle

    virtual Length openForRead();
    virtual void openForWrite(const Length&);
    virtual void openForAppend(const Length&);

    virtual long read(void*, long);
    virtual long write(const void*, long);
    virtual void close();
    // virtual void rewind();
    virtual void print(std::ostream&) const;
    virtual Length estimate();

    // From Streamable

    virtual void encode(Stream&) const;
    virtual const ReanimatorBase& reanimator() const { return reanimator_; }

// -- Class methods

    static  const ClassSpec&  classSpec()        { return classSpec_;}

private:

// -- Members

    std::string uri_;
    CircularBuffer buffer_;


// -- Methods

    void init();

    virtual size_t writeCallback(void *ptr, size_t size);

// -- Class members

    static  ClassSpec               classSpec_;
    static  Reanimator<URLHandle>  reanimator_;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
