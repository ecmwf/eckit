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
/// @date   June 2019

#ifndef eckit_filesystem_FileMode_h
#define eckit_filesystem_FileMode_h

#include <sys/types.h>
#include <sys/stat.h>

#include <string>
#include <iostream>

#include "eckit/utils/Tokenizer.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class FileMode {
public:

    FileMode(mode_t m = 0) :
        mode_(m) {
    }

    FileMode(const std::string& s) {
        mode_ = toMode(s);
    }

    FileMode& operator=(const std::string& s) {
        mode_ = toMode(s);
        return *this;
    }

    FileMode& operator=(mode_t m) {
        mode_ = m;
        return *this;
    }

    mode_t mode() const { return mode_; }
    std::string str() const { return toString(); }

    operator std::string() const { return str(); }
    operator mode_t() const { return mode_; }

    static int fromPath(const std::string& path);

private:  // methods

    friend std::ostream& operator<< (std::ostream& os, const FileMode& mode) { mode.print(os); return os; }

    void print(std::ostream& os) const {
        os << str();
    }

    void testAssign(const std::string& s, char got, char test, mode_t& mode, mode_t mask) const;

    mode_t toMode(const std::string& s) const;

    std::string toString() const;

private:           // members
    mode_t mode_;  ///< store the actual mode
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
