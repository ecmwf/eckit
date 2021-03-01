/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_io_AIOHandle_h
#define eckit_io_AIOHandle_h

#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"

namespace eckit {

struct AIOBuffer;

class AIOHandle : public DataHandle {

public:  // methods
    AIOHandle(const PathName& path, size_t count = 16, size_t buffsize = 1024 * 1024, bool fsync = false);

    virtual ~AIOHandle() override;

    virtual Length openForRead() override;
    virtual void openForWrite(const Length&) override;
    virtual void openForAppend(const Length&) override;

    virtual long read(void*, long) override;
    virtual long write(const void*, long) override;
    virtual void close() override;
    virtual void flush() override;
    virtual void rewind() override;
    virtual void print(std::ostream&) const override;

    virtual Length size() override;
    virtual Length estimate() override;
    virtual Offset position() override;

    virtual bool canSeek() const override { return false; }

private:  // methods
    size_t getFreeSlot();

protected:  // members
    PathName path_;

private:  // members
    std::vector<AIOBuffer*> buffers_;

    size_t used_;
    size_t count_;

    int fd_;
    off_t pos_;
    bool fsync_;


    virtual std::string title() const override;
};

}  // namespace eckit

#endif
