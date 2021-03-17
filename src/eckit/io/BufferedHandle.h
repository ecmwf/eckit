/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/BufferedHandle.h
// Manuel Fuentes - ECMWF Jul 96

#ifndef eckit_filesystem_BufferedHandle_h
#define eckit_filesystem_BufferedHandle_h

#include "eckit/io/Buffer.h"
#include "eckit/io/HandleHolder.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class BufferedHandle : public DataHandle, public HandleHolder {
public:
    /// Contructor, taking ownership

    BufferedHandle(DataHandle*, size_t = 1024 * 1024);

    /// Contructor, not taking ownership

    BufferedHandle(DataHandle&, size_t = 1024 * 1024);

    /// Destructor

    virtual ~BufferedHandle() override;

    // From DataHandle

    virtual Length openForRead() override;
    virtual void openForWrite(const Length&) override;
    virtual void openForAppend(const Length&) override;

    virtual long read(void*, long) override;
    virtual long write(const void*, long) override;
    virtual void close() override;
    virtual void flush() override;
    virtual void rewind() override;
    virtual void print(std::ostream&) const override;
    virtual void skip(const Length&) override;

    virtual Offset seek(const Offset&) override;
    virtual bool canSeek() const override { return handle().canSeek(); }


    virtual Length estimate() override;
    virtual Offset position() override;

    virtual DataHandle* clone() const override;

private:  // methods
    void bufferFlush();

private:  // members
    Buffer buffer_;
    size_t pos_;
    size_t size_;
    size_t used_;
    bool eof_;
    bool read_;
    Offset position_;

    virtual std::string title() const override;
    virtual void collectMetrics(const std::string& what) const override;

};

//-----------------------------------------------------------------------------

}  // namespace eckit


#endif
