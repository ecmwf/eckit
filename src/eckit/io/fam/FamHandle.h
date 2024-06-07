/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

/// @file   FamHandle.h
/// @author Metin Cakircali
/// @date   May 2024

#pragma once

#include "eckit/io/DataHandle.h"
#include "eckit/io/fam/FamObjectName.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class FamHandle: public DataHandle {
public:  // methods
    enum class Mode { CLOSED, READ, WRITE };

    FamHandle(FamObjectName name, const Offset& offset, const Length& length);

    FamHandle(FamObjectName name, const Offset& offset = 0);

    Length openForRead() override;

    void openForWrite(const Length& length) override;

    long read(void* buffer, long length) override;

    long write(const void* buffer, long length) override;

    void flush() override;

    void close() override;

    Length size() override;

    Length estimate() override;

    Offset position() override { return pos_; }

    Offset seek(const Offset& offset) override;

    auto canSeek() const -> bool override { return true; }

private:  // methods
    void print(std::ostream& out) const override;

    void open(Mode mode);

private:  // members
    FamObjectName name_;

    Offset pos_ {0};
    Length len_ {0};

    Mode mode_ {Mode::CLOSED};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
