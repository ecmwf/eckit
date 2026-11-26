// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   June 96

#ifndef eckit_io_StdFile_h
#define eckit_io_StdFile_h

#include <cstdio>

#include "eckit/filesystem/PathName.h"


namespace eckit {

class PathName;

/// Wrapper around a stdio FILE*
/// Use this for class members

class StdFile {
public:

    StdFile(const PathName& name, const std::string& mode = "r");

    StdFile(const StdFile&)            = delete;
    StdFile& operator=(const StdFile&) = delete;
    StdFile(StdFile&&)                 = delete;
    StdFile& operator=(StdFile&&)      = delete;

    /// @pre must have been closed
    ~StdFile();

    /// Get the FILE* but don't call fclose on it
    operator FILE*() { return file_; }

    bool isOpen() { return file_; }

    /// @throws on fclose failure
    void close() noexcept(false);

private:  // members

    FILE* file_;
};


/// Wrapper around a stdio FILE*
/// Use this for stack objects that automatically close

class AutoStdFile : public StdFile {
    void close() { StdFile::close(); }

public:

    AutoStdFile(const PathName& name, const std::string& mode = "r") : StdFile(name, mode) {}
    ~AutoStdFile() noexcept(false) { close(); }
};


}  // namespace eckit

#endif
