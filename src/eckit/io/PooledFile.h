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
/// @date   June 2019

#ifndef eckit_io_PooledFile_h
#define eckit_io_PooledFile_h

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/memory/NonCopyable.h"


namespace eckit {

class PoolFileEntry;

class PooledFile : private NonCopyable {
public:

    PooledFile(const PathName& name);

    /// @pre must have been closed
    ~PooledFile();

    void open();

    /// @throws on fclose failure
    void close() noexcept(false);

    long read(void*, long);

    off_t seek(off_t offset);
    off_t seekEnd();

    off_t rewind();

    int fileno() const;

    // for testing

    size_t nbOpens() const;
    size_t nbReads() const;
    size_t nbSeeks() const;

private:

    PathName name_;
    PoolFileEntry* entry_;
};


class PooledFileError : public FileError {
public:

    PooledFileError(const std::string& file, const std::string& msg, const CodeLocation& loc);
};

}  // namespace eckit

#endif
