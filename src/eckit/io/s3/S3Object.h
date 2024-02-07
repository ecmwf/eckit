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

/// @file   S3Object.h
/// @author Metin Cakircali
/// @date   Jan 2024

#pragma once

#include "eckit/io/s3/S3Macros.h"

#include <memory>
#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

///>>> What is S3ObjectBase for? Why do we not directly implement S3Object?

class S3ObjectBase {
public:  // methods
    NO_COPY_NO_MOVE(S3ObjectBase)

    explicit S3ObjectBase(const std::string& name): name_(name) { }

    virtual ~S3ObjectBase() = default;

    friend std::ostream& operator<<(std::ostream& out, const S3ObjectBase& obj) {
        obj.print(out);
        return out;
    }

    virtual bool create(const std::string& name) = 0;

    virtual bool remove() = 0;

    virtual bool open() = 0;

    virtual long read(void* buffer, long length, off_t offset) const = 0;

    virtual long write(const void* buffer, long length, off_t offset) const = 0;

    virtual bool close() = 0;

private:  // methods
    virtual void print(std::ostream& out) const = 0;

private:  // members
    std::string name_;

    bool isOpen_ {false};
};

//----------------------------------------------------------------------------------------------------------------------

class S3Object: public S3ObjectBase {
public:  // methods
    NO_COPY_NO_MOVE(S3Object)

    S3Object(const std::string& name);

    ~S3Object() override;

    bool create(const std::string& name) override;

    bool remove() override;

    bool open() override;

    long read(void* buffer, long length, off_t offset) const override;

    long write(const void* buffer, long length, off_t offset) const override;

    bool close() override;

    long size();

private:  // methods
    void print(std::ostream& out) const override;

private:  // members
    class S3ObjectInternal;
    std::unique_ptr<S3ObjectInternal> impl_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
