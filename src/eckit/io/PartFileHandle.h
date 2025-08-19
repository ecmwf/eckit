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
/// @author Emanuele Danovaro
/// @author Simon Smart
/// @author Tiago Quintino
/// @date   May 1996

#ifndef eckit_filesystem_PartFileHandle_h
#define eckit_filesystem_PartFileHandle_h

#include <memory>

#include "eckit/filesystem/PathName.h"
#include "eckit/io/DataHandle.h"
#include "eckit/types/Types.h"

namespace eckit {

class PooledHandle;

//----------------------------------------------------------------------------------------------------------------------

class PartFileHandle : public DataHandle {
public:  // methods

    PartFileHandle(const PathName&, const OffsetList&, const LengthList&);
    PartFileHandle(const PathName&, const Offset&, const Length&);
    explicit PartFileHandle(Stream&);

    ~PartFileHandle() override;

    const PathName& path() const { return path_; }

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
    bool merge(DataHandle*) override;
    bool compress(bool = false) override;
    Length size() override;
    Length estimate() override;

    void restartReadFrom(const Offset& from) override;
    Offset position() override;
    Offset seek(const Offset&) override;
    bool canSeek() const override;

    void selectMover(MoverTransferSelection&, bool read) const override;

    std::string title() const override;
    std::string metricsTag() const override;


    bool moveable() const override { return true; }
    DataHandle* clone() const override;

    // From Streamable

    void encode(Stream&) const override;
    const ReanimatorBase& reanimator() const override { return reanimator_; }

    // -- Class methods

    static const ClassSpec& classSpec() { return classSpec_; }

private:  // members

    PathName path_;
    std::unique_ptr<PooledHandle> handle_;
    long long pos_;
    Ordinal index_;
    OffsetList offset_;
    LengthList length_;

private:  // methods

    long read1(char*, long);

    static ClassSpec classSpec_;
    static Reanimator<PartFileHandle> reanimator_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
