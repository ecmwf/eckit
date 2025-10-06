/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File FileBase.h
// Baudouin Raoult - ECMWF Jun 97

#ifndef eckit_FileBase_h
#define eckit_FileBase_h

#include "eckit/eckit.h"

#include "eckit/filesystem/PathName.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

template <class T>
class FileBase {
public:

    // -- Exceptions
    // None

    // -- Contructors

    FileBase(const PathName&);

    // -- Destructor

    ~FileBase();

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    bool read(long, T&);
    void write(long, const T&);

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:

    // -- Members
    // None

    // -- Methods

    // void print(std::ostream&) const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:

    // No copy allowed

    FileBase(const FileBase<T>&);
    FileBase<T>& operator=(const FileBase<T>&);

    // -- Members

    struct Record {
        T data_;
        bool valid_;
    };

    int fd_;
    PathName path_;
    Record buffer_;
    off_t pos_;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends

    // friend std::ostream& operator<<(std::ostream& s,const FileBase& p)
    //	{ p.print(s); return s; }
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#include "eckit/io/FileBase.cc"

#endif
