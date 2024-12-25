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

#include <memory>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class FamHandle: public DataHandle {
public:  // methods
    enum class Mode { CLOSED, READ, WRITE };

    FamHandle(const FamObjectName& name, const Offset& position, const Length& length, bool overwrite);

    FamHandle(const FamObjectName& name, bool overwrite = false);

    Length openForRead() override;

    void openForWrite(const Length& length) override;

    long read(void* buffer, long length) override;

    long write(const void* buffer, long length) override;

    void flush() override;

    void close() override;

    Offset seek(const Offset& offset) override;

    auto canSeek() const -> bool override { return true; }

    Offset position() override { return pos_; }

    Length estimate() override { return len_; }

    Length size() override;

private:  // methods
    void open(Mode mode);

    void print(std::ostream& out) const override;

private:  // members
    const FamObjectName name_;

    const bool overwrite_ {false};

    Offset pos_ {0};
    Length len_ {0};

    Mode mode_ {Mode::CLOSED};

    std::unique_ptr<FamObject> handle_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
