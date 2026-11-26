// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   June 1996

#ifndef eckit_io_StdPipe_h
#define eckit_io_StdPipe_h

#include <cstdio>
#include <string>

#include "eckit/io/AutoCloser.h"

namespace eckit {

/// Simple wrapper for pipes

class StdPipe {
public:

    StdPipe(const std::string& name, const std::string& mode = "r");

    StdPipe(const StdPipe&)            = delete;
    StdPipe& operator=(const StdPipe&) = delete;
    StdPipe(StdPipe&&)                 = delete;
    StdPipe& operator=(StdPipe&&)      = delete;

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
