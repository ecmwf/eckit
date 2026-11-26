// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   June 2019

#ifndef eckit_io_PooledFile_h
#define eckit_io_PooledFile_h

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"


namespace eckit {

class PoolFileEntry;

class PooledFile {
public:

    PooledFile(const PathName& name);

    PooledFile(const PooledFile&)            = delete;
    PooledFile& operator=(const PooledFile&) = delete;
    PooledFile(PooledFile&&)                 = delete;
    PooledFile& operator=(PooledFile&&)      = delete;

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
