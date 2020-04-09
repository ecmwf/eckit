/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Dec 2013

#ifndef eckit_filesystem_RawFileHandle_h
#define eckit_filesystem_RawFileHandle_h

#include "eckit/io/DataHandle.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RawFileHandle : public DataHandle {
public:

    RawFileHandle(const std::string& path, bool overwrite = false);

    ~RawFileHandle();

    virtual Length openForRead();
    virtual void openForWrite(const Length&);
    virtual void openForAppend(const Length&);

    virtual long   read(void*,long);
    virtual long   write(const void*,long);
    virtual void   close();
    virtual void   print(std::ostream&) const;

    virtual Length size();
    virtual Length estimate();
    virtual Offset position();
    virtual Offset seek(const Offset&);
    virtual void skip(const Length &);

    virtual void encode(Stream&) const;

private:

    std::string path_;
    bool overwrite_;
    int fd_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
