/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/FileChannel.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class FileBuffer : public std::filebuf {
public:

    FileBuffer( const LocalPathName& file ) : std::filebuf()
    {
        this->open(file.c_str(),std::ios::out|std::ios::trunc);
    }

    ~FileBuffer() { this->close(); }
};

//-----------------------------------------------------------------------------

FileChannel::FileChannel(const LocalPathName &file)
    : Channel( new FileBuffer(file) )
{
}

//-----------------------------------------------------------------------------

} // namespace eckit
