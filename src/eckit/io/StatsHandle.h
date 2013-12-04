/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Partio/FileHandle.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_filesystem_StatsHandle_h
#define eckit_filesystem_StatsHandle_h

#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/types/Types.h"
#include "eckit/utils/Timer.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class StatsHandle : public DataHandle {
public:

    // -- Contructors

    StatsHandle(DataHandle& handle);

    // -- Destructor

    ~StatsHandle();

    // -- Methods

    // -- Overridden methods

    // From DataHandle


    virtual void print(std::ostream& s) const;

    virtual Length openForRead();
    virtual void openForWrite(const Length&);
    virtual void openForAppend(const Length&);

    virtual long read(void*,long);
    virtual long write(const void*,long);
    virtual void close();
    virtual void flush();

    virtual Length estimate();
    virtual Offset position();
    virtual Offset seek(const Offset&);
    virtual void skip(const Length &);

    virtual void rewind();
    virtual void restartReadFrom(const Offset&);
    virtual void restartWriteFrom(const Offset&);

    virtual DataHandle* clone() const;

    virtual Length saveInto(DataHandle& other, TransferWatcher& watcher);

    virtual std::string name() const;


    virtual bool compress(bool);
    virtual bool merge(DataHandle*);
    virtual bool isEmpty() const;


    virtual bool moveable() const;
    virtual void toLocal(Stream& s) const;

    virtual DataHandle* toLocal();

    virtual void toRemote(Stream& s) const;
    virtual void cost(std::map<std::string,Length>&, bool) const;
    virtual std::string title() const;



private:

    // -- Members

    DataHandle&        handle_;
    size_t reads_;
    size_t seeks_;
    size_t positions_;
    size_t writes_;
    unsigned long long bytesRead_;
    unsigned long long bytesWritten_;

    Timer timer_;

    double readTime_;
    double writeTime_;
    double seekTime_;


    // -- Methods



};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
