/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Partio/FileHandle.h
// Baudouin Raoult - ECMWF Decembre 2013

#ifndef eckit_filesystem_StatsHandle_h
#define eckit_filesystem_StatsHandle_h

#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/HandleHolder.h"
#include "eckit/log/Timer.h"
#include "eckit/types/Types.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class StatsHandle : public DataHandle, public HandleHolder {
public:
    // -- Contructors

    StatsHandle(DataHandle& handle);
    StatsHandle(DataHandle* handle);

    // -- Destructor

    ~StatsHandle();

    // -- Methods

    // -- Overridden methods

    // From DataHandle


    void print(std::ostream& s) const override;

    Length openForRead() override;
    void openForWrite(const Length&) override;
    void openForAppend(const Length&) override;

    long read(void*, long) override;
    long write(const void*, long) override;
    void close() override;
    void flush() override;

    Length estimate() override;
    Offset position() override;
    Offset seek(const Offset&) override;
    bool canSeek() const override { return handle().canSeek(); }
    void skip(const Length&) override;

    void rewind() override;
    void restartReadFrom(const Offset&) override;
    void restartWriteFrom(const Offset&) override;

    DataHandle* clone() const override;

    using DataHandle::saveInto;
    Length saveInto(DataHandle& other, TransferWatcher& watcher) override;

    std::string name() const override;


    bool compress(bool) override;
    bool merge(DataHandle*) override;
    bool isEmpty() const override;


    bool moveable() const override;

    void toLocal(Stream& s) const override;

    DataHandle* toLocal() override;

    void toRemote(Stream& s) const override;
    void selectMover(MoverTransferSelection&, bool read) const override;

    std::string title() const override;
    void collectMetrics(const std::string& what) const override;  // Tag for metrics collection


private:
    // -- Members

    size_t reads_;
    size_t seeks_;
    //    size_t positions_; // unused
    size_t writes_;

    unsigned long long bytesRead_;
    unsigned long long bytesWritten_;

    Timer timer_;

    double readTime_;
    double writeTime_;
    double seekTime_;
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
