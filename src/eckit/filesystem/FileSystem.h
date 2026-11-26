// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
