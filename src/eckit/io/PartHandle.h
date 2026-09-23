// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File Partio/FileHandle.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_filesystem_PartHandle_h
#define eckit_filesystem_PartHandle_h

#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/HandleHolder.h"
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
