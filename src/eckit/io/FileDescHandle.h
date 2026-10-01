// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
