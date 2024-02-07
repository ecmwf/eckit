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

#include <iostream>
#include <memory>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

///>>> Note DataHandle derives from eckit::NonCopyable, so there is no need to
///>>> make that more explicit here.

class S3Handle: public DataHandle {
public:
    NO_COPY_NO_MOVE(S3Handle)

    // NOTE: @Nicolau we will try S3 stream later which is not here yet

    explicit S3Handle(const PathName& name);

    explicit S3Handle(const PathName& name, const eckit::Offset& offset);

    ~S3Handle() override;

    void print(std::ostream& out) const override;

public:
    eckit::Length openForRead() override;

    void openForWrite(const eckit::Length& length) override;

    void openForAppend(const eckit::Length& length) override;

    long read(void* buffer, long length) override;

    long write(const void* buffer, long length) override;

    void flush() override;

    void close() override;

    eckit::Length size() override;

    eckit::Offset position() override;

    eckit::Offset seek(const eckit::Offset& offset) override;

    std::string title() const override;

    bool canSeek() const override { return true; }

private:  // members
    class S3HandleInternal;
    std::unique_ptr<S3HandleInternal> impl_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
