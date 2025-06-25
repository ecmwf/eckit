/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Olivier Iffrig
/// @date   Oct 2019

#ifndef eckit_io_PooledFileDescriptor_h
#define eckit_io_PooledFileDescriptor_h

#include <memory>

#include "eckit/filesystem/PathName.h"
#include "eckit/io/PooledFile.h"


namespace eckit {

class PooledFileDescriptor {
public:

    PooledFileDescriptor(const PathName& path, bool readOnly);

    ~PooledFileDescriptor();

    void open();
    void close();

    ssize_t read(void*, size_t);
    ssize_t write(const void*, size_t);

    void sync();

    off_t seek(off_t offset);
    off_t seekEnd();

    int fileno() const { return fd_; }

private:

    PathName path_;
    std::unique_ptr<PooledFile> file_;
    int fd_;
    bool readOnly_;
};

}  // namespace eckit

#endif
