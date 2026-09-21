// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Tiago Quintino

#ifndef eckit_log_CodeLocation_h
#define eckit_log_CodeLocation_h

#include <iosfwd>
#include <ostream>
#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class CodeLocation {

public:  // methods

    friend std::ostream& operator<<(std::ostream& s, const CodeLocation& loc) {
        loc.print(s);
        return s;
    }

    /// Empty contructor
    CodeLocation() : line_(0), file_{nullptr}, func_{nullptr} {}

    /// Full Contructor
    CodeLocation(const char* file, int line, const char* func) : line_(line), file_(file), func_(func) {}

    /// @return as std::string
    std::string asString() const;

    /// conversion operator
    operator std::string() const;

    /// conversion to bool for checking if location was set
    operator bool() const;

    /// accessor to line
    int line() const { return line_; }
    /// accessor to file
    const char* file() const { return file_; }
    /// accessor to function
    const char* func() const { return func_; }

private:  // members

    int line_;
    const char* file_;
    const char* func_;

protected:  // methods

    void print(std::ostream&) const;
};

// Macros

#define Here() ::eckit::CodeLocation(__FILE__, __LINE__, __func__)

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
