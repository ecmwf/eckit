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
/// @date   June 1996

#ifndef eckit_io_StdPipe_h
#define eckit_io_StdPipe_h

#include <cstdio>
#include <string>

#include "eckit/io/AutoCloser.h"
#include "eckit/memory/NonCopyable.h"

namespace eckit {

/// Simple wrapper for pipes

class StdPipe : private NonCopyable {
public:

    StdPipe(const std::string& name, const std::string& mode = "r");

    /// @pre must have been closed
    ~StdPipe();

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

class AutoStdPipe : public StdPipe {
public:

    AutoStdPipe(const std::string& name, const std::string& mode = "r") : StdPipe(name, mode) {}
    ~AutoStdPipe() noexcept(false) { close(); }
};


}  // namespace eckit

#endif
