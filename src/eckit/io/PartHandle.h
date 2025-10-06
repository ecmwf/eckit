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
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_filesystem_PartHandle_h
#define eckit_filesystem_PartHandle_h

#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/HandleHolder.h"

#include "eckit/filesystem/PathName.h"
#include "eckit/types/Types.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class PartHandle : public DataHandle, public HandleHolder {
public:

    // -- Contructors

    PartHandle(DataHandle& handle, const OffsetList&, const LengthList&);
    PartHandle(DataHandle& handle, const Offset&, const Length&);

    PartHandle(DataHandle* handle, const OffsetList&, const LengthList&);
    PartHandle(DataHandle* handle, const Offset&, const Length&);

    PartHandle(Stream&);

    // -- Destructor

    ~PartHandle();

    // -- Methods

    // -- Overridden methods

    // From DataHandle

    Length openForRead() override;
    void openForWrite(const Length&) override;
    void openForAppend(const Length&) override;

    long read(void*, long) override;
    long write(const void*, long) override;
    void close() override;
    void rewind() override;
    void print(std::ostream&) const override;


    Length estimate() override;
    void restartReadFrom(const Offset& from) override;

    // From Streamable

    void encode(Stream&) const override;
    const ReanimatorBase& reanimator() const override { return reanimator_; }

    // -- Class methods

    static const ClassSpec& classSpec() { return classSpec_; }

private:

    // -- Members

    long long pos_;
    Ordinal index_;
    OffsetList offset_;
    LengthList length_;

    // -- Methods

    long read1(char*, long);

    // -- Class members

    static ClassSpec classSpec_;
    static Reanimator<PartHandle> reanimator_;
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
