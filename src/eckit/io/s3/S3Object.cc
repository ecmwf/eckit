/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/s3/S3Object.h"

#include "eckit/io/s3/S3Exception.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class S3Object::S3ObjectInternal {
public:
    // S3ObjectInternal(const std::string& name): name_(name) { }

    S3ObjectInternal()  = default;
    ~S3ObjectInternal() = default;

    // private:
    //     const std::string& name_;
};

//----------------------------------------------------------------------------------------------------------------------

S3Object::S3Object(const std::string& name): S3ObjectBase(name), impl_(std::make_unique<S3ObjectInternal>()) { }

S3Object::~S3Object() { }

//----------------------------------------------------------------------------------------------------------------------

bool S3Object::create(const std::string& name) {
    NOTIMP;
    return false;
}

bool S3Object::remove() {
    NOTIMP;
    return false;
}

bool S3Object::open() {
    NOTIMP;
    return false;
}

long S3Object::read(void* buffer, long length, off_t offset) const {
    NOTIMP;
    return 0;
}

long S3Object::write(const void* buffer, long length, off_t offset) const {
    NOTIMP;
    return 0;
}

bool S3Object::close() {
    NOTIMP;
    return false;
}

long S3Object::size() {
    NOTIMP;
    return 0;
}

void S3Object::print(std::ostream& out) const { }

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
