/*
 * Copyright 2024- European Centre for Medium-Range Weather Forecasts (ECMWF).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/// @file   S3Handle.h
/// @author Metin Cakircali
/// @date   Jan 2024

#pragma once

#include "eckit/io/DataHandle.h"
#include "eckit/io/s3/S3Macros.h"
#include "eckit/io/s3/S3Name.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class S3Handle: public DataHandle {
public:  // methods
    NO_COPY_NO_MOVE(S3Handle)

    explicit S3Handle(const S3Name& name);

    explicit S3Handle(const S3Name& name, const Offset& offset);

    ~S3Handle() override;

    void print(std::ostream& out) const override;

public:  // methods
    Length openForRead() override;

    void openForWrite(const Length& length) override;

    void openForAppend(const Length& length) override;

    long read(void* buffer, long length) override;

    long write(const void* buffer, long length) override;

    void flush() override;

    void close() override;

    Length size() override;

    Offset position() override;

    Offset seek(const Offset& offset) override;

    std::string title() const override;

    bool canSeek() const override { return true; }

private:  // members
    S3Name name_;
    Offset pos_ {0};
    // bool   canWrite_ {false};
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
