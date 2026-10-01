// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

    BufferedHandle(DataHandle*, size_t = 1024 * 1024, bool opened = false);

    /// Contructor, not taking ownership

    BufferedHandle(DataHandle&, size_t = 1024 * 1024, bool opened = false);

    /// Destructor

    ~BufferedHandle() override;

    // From DataHandle

    Length openForRead() override;
    void openForWrite(const Length&) override;
    void openForAppend(const Length&) override;

    long read(void*, long) override;
    long write(const void*, long) override;
    void close() override;
    void flush() override;
    void rewind() override;
    void print(std::ostream&) const override;
    void skip(const Length&) override;

    Offset seek(const Offset&) override;
    bool canSeek() const override { return handle().canSeek(); }


    Length estimate() override;
    Offset position() override;

    DataHandle* clone() const override;

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
    bool opened_;

    std::string title() const override;
    void collectMetrics(const std::string& what) const override;
};

//-----------------------------------------------------------------------------

}  // namespace eckit


#endif
