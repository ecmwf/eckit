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


    virtual void print(std::ostream& s) const override;

    virtual Length openForRead() override;
    virtual void openForWrite(const Length&) override;
    virtual void openForAppend(const Length&) override;

    virtual long read(void*, long) override;
    virtual long write(const void*, long) override;
    virtual void close() override;
    virtual void flush() override;

    virtual Length estimate() override;
    virtual Offset position() override;
    virtual Offset seek(const Offset&) override;
    virtual bool canSeek() const override { return handle().canSeek(); }
    virtual void skip(const Length&) override;

    virtual void rewind() override;
    virtual void restartReadFrom(const Offset&) override;
    virtual void restartWriteFrom(const Offset&) override;

    virtual DataHandle* clone() const override;

    virtual Length saveInto(DataHandle& other, TransferWatcher& watcher) override;

    virtual std::string name() const override;


    virtual bool compress(bool) override;
    virtual bool merge(DataHandle*) override;
    virtual bool isEmpty() const override;


    virtual bool moveable() const override;
    virtual const std::set<std::string>& requiredMoverAttributes() const override;
    virtual void toLocal(Stream& s) const override;

    virtual DataHandle* toLocal() override;

    virtual void toRemote(Stream& s) const override;
    virtual void cost(std::map<std::string, Length>&, bool) const override;
    virtual std::string title() const override;
    virtual void collectMetrics(const std::string& what) const override; // Tag for metrics collection


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
