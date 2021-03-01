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
/// @date   June 2019

#ifndef eckit_io_PooledHandle_h
#define eckit_io_PooledHandle_h

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/DataHandle.h"


namespace eckit {

class PoolHandleEntry;

class PooledHandle : public DataHandle {
public:
    PooledHandle(const PathName& name);

    /// @pre must have been closed
    ~PooledHandle();

    virtual Length openForRead() override;
    virtual void openForWrite(const Length&) override;
    virtual void openForAppend(const Length&) override;

    virtual long read(void*, long) override;
    virtual long write(const void*, long) override;
    virtual void close() override;
    virtual Offset seek(const Offset&) override;
    virtual bool canSeek() const override { return true; }
    virtual void hash(MD5& md5) const override;
    virtual Offset position() override;

    // for testing

    size_t nbOpens() const;
    size_t nbReads() const;
    size_t nbSeeks() const;

private:
    PathName path_;
    PoolHandleEntry* entry_;

    virtual void print(std::ostream& s) const override;
};

}  // namespace eckit

#endif
