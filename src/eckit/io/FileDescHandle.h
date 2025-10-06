/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/FileDescHandle.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_filesystem_FileDescHandle_h
#define eckit_filesystem_FileDescHandle_h

#include "eckit/io/DataHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class FileDescHandle : public DataHandle {
public:

    // -- Contructors

    FileDescHandle(int, bool close = false);

    // -- Destructor

    ~FileDescHandle();

    // -- Overridden methods

    // From DataHandle

    Length openForRead() override;
    void openForWrite(const Length&) override;
    void openForAppend(const Length&) override;

    long read(void*, long) override;
    long write(const void*, long) override;
    void close() override;
    void print(std::ostream&) const override;

    Offset position() override;
    Offset seek(const Offset&) override;
    bool canSeek() const override { return true; }
    void skip(const Length&) override;

    // From Streamable

    void encode(Stream&) const override;

    // -- Class methods


private:

    // -- Members

    int fd_;
    bool close_;

    // -- Class members
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
