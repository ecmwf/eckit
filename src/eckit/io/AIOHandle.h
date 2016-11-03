/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_AIOHandle_h
#define eckit_AIOHandle_h

#include <aio.h>

#include "eckit/eckit.h"

#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/filesystem/PathName.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class AIOHandle : public DataHandle {

public: // methods


    /// Contructor

    AIOHandle(const PathName& path, size_t count = 64, size_t = 1024 * 1024, bool fsync = false);

    /// Destructor

    virtual ~AIOHandle();

    // From DataHandle

    virtual Length openForRead();
    virtual void openForWrite(const Length&);
    virtual void openForAppend(const Length&);

    virtual long read(void*, long);
    virtual long write(const void*, long);
    virtual void close();
    virtual void flush();
    virtual void rewind();
    virtual void print(std::ostream&) const;

    virtual Length estimate();
    virtual Offset position();


protected: // members

    PathName                   path_;

private: // members

    std::vector<Buffer*>       buffers_;
    std::vector<const aiocb*>  aiop_;
    std::vector<aiocb>         aio_;
    std::vector<long>          len_;
    std::vector<bool>          active_;

    size_t                     used_;
    size_t                     count_;

    int                        fd_;
    off_t                      pos_;
    bool                       fsync_;


    virtual std::string title() const;

// -- Class members


};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
