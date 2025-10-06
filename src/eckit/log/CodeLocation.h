/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino

#ifndef eckit_log_CodeLocation_h
#define eckit_log_CodeLocation_h

#include <iosfwd>
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
    CodeLocation() : line_(0), file_(nullptr), func_(nullptr) {}

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
