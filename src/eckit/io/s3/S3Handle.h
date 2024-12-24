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
 * This software was developed as part of the EC H2020 funded project IO-SEA
 * (Project ID: 955811) iosea-project.eu
 */

/// @file   S3Handle.h
/// @author Metin Cakircali
/// @date   Jan 2024

#pragma once

#include "eckit/io/DataHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/s3/S3ObjectName.h"

#include <cstdint>
#include <iosfwd>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class S3Handle : public DataHandle {
public:  // methods
    enum class Mode : std::uint8_t { CLOSED, READ, WRITE };

    S3Handle(const S3ObjectName& name, const Offset& offset);

    explicit S3Handle(const S3ObjectName& name);

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
    S3ObjectName name_;

    Offset pos_ {0};

    Mode mode_ {Mode::CLOSED};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
