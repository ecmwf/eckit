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
/// @date Nov 2001

#ifndef eckit_FileSystem_h
#define eckit_FileSystem_h

#include "eckit/filesystem/PathName.h"

namespace eckit {

class JSON;

//----------------------------------------------------------------------------------------------------------------------

class FileSystem {
public:

    FileSystem(const PathName&);

    ~FileSystem();

    void update();

    std::string name() const { return name_; }
    std::string mountPoint() const;

    unsigned long long total() const;
    unsigned long long avail() const;
    long usage() const;
    bool offLine() const { return offLine_; }

protected:

    void json(eckit::JSON& s) const;

    void print(std::ostream&) const;

private:

    PathName name_;
    PathName mount_;
    FileSystemSize fs_;
    bool offLine_;

    friend std::ostream& operator<<(std::ostream& s, const FileSystem& p) {
        p.print(s);
        return s;
    }

    friend JSON& operator<<(JSON& s, const FileSystem& v) {
        v.json(s);
        return s;
    }
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
