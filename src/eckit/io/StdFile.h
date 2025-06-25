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
/// @date   June 96

#ifndef eckit_io_StdFile_h
#define eckit_io_StdFile_h

#include <cstdio>

#include "eckit/filesystem/PathName.h"
#include "eckit/memory/NonCopyable.h"


namespace eckit {

class PathName;

/// Wrapper around a stdio FILE*
/// Use this for class members

class StdFile : private NonCopyable {
public:

    StdFile(const PathName& name, const std::string& mode = "r");

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
