/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/AsyncHandle.h
// Baudouin Raoult - ECMWF Dec 17

#ifndef eckit_io_AsyncHandle_h
#define eckit_io_AsyncHandle_h

#include <deque>

#include "eckit/io/Buffer.h"
#include "eckit/io/HandleHolder.h"
#include "eckit/thread/MutexCond.h"
#include "eckit/thread/ThreadControler.h"


namespace eckit {

//-----------------------------------------------------------------------------

class AsyncHandle : public DataHandle, public HandleHolder {
public:

    /// Contructor, taking ownership

    AsyncHandle(DataHandle*, size_t = 1024 * 1024, size_t rounding = 64 * 1024);

    /// Contructor, not taking ownership

    AsyncHandle(DataHandle&, size_t = 1024 * 1024, size_t rounding = 64 * 1024);

    /// Destructor

    ~AsyncHandle() override;

    // -- Operators


    // -- Overridden methods

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

    Length estimate() override;
    Offset position() override;

    DataHandle* clone() const override;

    // From Streamable

#if 0
    void encode(Stream&) const override;
    const ReanimatorBase& reanimator() const override { return reanimator_; }
#endif

    // -- Class methods

#if 0
    static  const ClassSpec&  classSpec()        { return classSpec_;}
#endif

private:  // methods

private:  // members

    std::string message_;
    size_t maxSize_;
    size_t used_;
    size_t rounding_;
    bool error_;

    MutexCond cond_;

    std::deque<std::pair<size_t, Buffer*> > buffers_;

    ThreadControler thread_;  // must be last

    std::string title() const override;
    void collectMetrics(const std::string& what) const override;

    // -- Class members

    friend class AsyncHandleWriter;

#if 0
    static  ClassSpec                 classSpec_;
    static  Reanimator<AsyncHandle>  reanimator_;
#endif
};

//-----------------------------------------------------------------------------

}  // namespace eckit


#endif
