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
///
/// @date May 96

#ifndef eckit_io_DataHandle_h
#define eckit_io_DataHandle_h

#include <cstdio>

#include "eckit/filesystem/PathName.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/io/TransferWatcher.h"
#include "eckit/serialisation/Streamable.h"

namespace eckit {

class MD5;
class Metrics;
class MoverTransferSelection;

//----------------------------------------------------------------------------------------------------------------------

class RestartTransfer {
    Offset from_;

public:

    RestartTransfer(const Offset& from) : from_(from) {}
    const Offset& from() const { return from_; }
};


//----------------------------------------------------------------------------------------------------------------------

class DataHandle : public Streamable {
public:

    friend std::ostream& operator<<(std::ostream& s, const DataHandle& handle) {
        handle.print(s);
        return s;
    }

    friend MD5& operator<<(MD5& s, const DataHandle& handle) {
        handle.hash(s);
        return s;
    }

    // -- Contructors

    DataHandle();
    DataHandle(Stream&);

    // -- Destructor

    ~DataHandle() override {}

    // -- Methods

    virtual void print(std::ostream& s) const = 0;

    virtual Length openForRead();               //< Returns estimated length
    virtual void openForWrite(const Length&);   //< Receive estimated length
    virtual void openForAppend(const Length&);  //< Receive estimated length

    virtual long read(void*, long);
    virtual long write(const void*, long);
    virtual void close();
    virtual void flush();

    virtual Length size();
    virtual Length estimate();
    virtual Offset position();
    virtual Offset seek(const Offset&);
    virtual bool canSeek() const;
    virtual void skip(const Length&);

    virtual void rewind();
    virtual void restartReadFrom(const Offset&);
    virtual void restartWriteFrom(const Offset&);

    virtual DataHandle* clone() const;

    /// Save into an other datahandle
    virtual Length saveInto(DataHandle&, TransferWatcher& = TransferWatcher::dummy());

    /// Save into a file
    virtual Length saveInto(const PathName&, TransferWatcher& = TransferWatcher::dummy());

    /// Quiet version of saveInto
    /// Does not support progess, restart and double buffering
    virtual Length copyTo(DataHandle&, long bufsize = -1, Length maxsize = -1,
                          TransferWatcher& = TransferWatcher::dummy());


    // /// Append to an other datahandle
    // virtual Length appendTo(DataHandle&, const std::string& metricsPrefix="");

    // /// Append to a file
    // virtual Length appendTo(const PathName&, const std::string& metricsPrefix="");

    virtual std::string name() const;

    /// Create a FILE* from this handle
    FILE* openf(const char* mode, bool delete_on_close = false);
    FILE* openf(bool delete_on_close = false);

    /// Compare bytes
    bool compare(DataHandle&);

    // Merge-in an other datahandle

    virtual bool compress(bool = false) { return false; }
    virtual bool merge(DataHandle*) { return false; }
    virtual bool isEmpty() const { return false; }


    // For remote data movers

    virtual bool moveable() const { return false; }
    virtual void toLocal(Stream& s) const;

    virtual DataHandle* toLocal();

    virtual void toRemote(Stream& s) const;

    virtual void selectMover(MoverTransferSelection&, bool read) const;

    virtual std::string title() const;
    virtual std::string metricsTag() const;
    virtual void collectMetrics(const std::string& what) const;  // Tag for metrics collection

    // This is the MD5 of the Handle, not the data it points to

    virtual void hash(MD5& md5) const;

    // This DataHandle can doubleBufferOK

    virtual bool doubleBufferOK() const { return true; }

    // -- Overridden methods

    // From Streamble

    void encode(Stream&) const override;
    const ReanimatorBase& reanimator() const override { return reanimator_; }

    // -- Class methods

    static const ClassSpec& classSpec() { return classSpec_; }

private:

    // -- Class members

    static ClassSpec classSpec_;
    static Reanimator<DataHandle> reanimator_;
};

//----------------------------------------------------------------------------------------------------------------------

class AutoClose {
    DataHandle& handle_;

public:

    AutoClose(DataHandle& handle) : handle_(handle) {}
    ~AutoClose() noexcept(false);
};

//----------------------------------------------------------------------------------------------------------------------

template <>
Streamable* Reanimator<DataHandle>::ressucitate(Stream& s) const
#ifdef IBM
{
    return 0;
}
#else
    ;
#endif


//----------------------------------------------------------------------------------------------------------------------


}  // namespace eckit

#endif
