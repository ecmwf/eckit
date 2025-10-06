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

#include <sys/stat.h>
#include <sys/types.h>

#include <iosfwd>
#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

class PathName;
//----------------------------------------------------------------------------------------------------------------------

class FileMode {
public:

    FileMode(mode_t m = 0);

    FileMode(const std::string& s);

    FileMode& operator=(const std::string& s);

    FileMode& operator=(mode_t m);

    bool operator==(const FileMode& other) const;


    mode_t mode() const { return mode_; }
    std::string str() const { return toString(); }

    mode_t mask(const FileMode& target = FileMode(0666)) const;

    static FileMode fromPath(const PathName& path);

private:  // methods

    friend std::ostream& operator<<(std::ostream& os, const FileMode& mode) {
        mode.print(os);
        return os;
    }

    void print(std::ostream& os) const;

    void testAssign(const std::string& s, char got, char test, mode_t& mode, mode_t mask) const;

    mode_t toMode(const std::string& s) const;

    std::string toString() const;

private:  // members

    mode_t mode_;  ///< store the actual mode
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
