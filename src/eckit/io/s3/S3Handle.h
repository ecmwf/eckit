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
#include "eckit/io/s3/S3Name.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class S3Handle: public DataHandle {
public:  // methods
    explicit S3Handle(const S3Name& name);

    S3Handle(const S3Name& name, const Offset& offset);

    Length openForRead() override;

    void openForWrite(const Length& length) override;

    void openForAppend(const Length& length) override;

    long read(void* buffer, long length) override;

    long write(const void* buffer, long length) override;

    void flush() override;

    void close() override;

    Length size() override;

    Length estimate() override;

    Offset position() override;

    Offset seek(const Offset& offset) override;

    bool canSeek() const override { return true; }

private:  // methods
    void print(std::ostream& out) const override;

    void open(bool canWrite);

private:  // members
    const S3Name name_;

    Offset pos_ {0};

    bool open_ {false};
    bool canWrite_ {false};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
