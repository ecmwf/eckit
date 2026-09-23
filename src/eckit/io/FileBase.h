// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
