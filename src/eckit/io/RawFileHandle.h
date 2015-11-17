/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/RawFileHandle.h
// Baudouin Raoult - ECMWF Dec 2013

#ifndef eckit_filesystem_RawFileHandle_h
#define eckit_filesystem_RawFileHandle_h

#include "eckit/io/DataHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class RawFileHandle : public DataHandle {
public:

    // -- Contructors

    RawFileHandle(const std::string&,bool = false);


    // -- Destructor

    ~RawFileHandle();

    // -- Overridden methods

    // From DataHandle

    virtual Length openForRead();
    virtual void openForWrite(const Length&);
    virtual void openForAppend(const Length&);

    virtual long   read(void*,long);
    virtual long   write(const void*,long);
    virtual void   close();
    virtual void   print(std::ostream&) const;

    virtual Offset position();
    virtual Offset seek(const Offset&);
    virtual void skip(const Length &);

    // From Streamable

    virtual void encode(Stream&) const;

    // -- Class methods


private:

    // -- Members

    std::string path_;
    bool overwrite_;
    int fd_;

    // -- Class members

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
