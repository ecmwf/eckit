/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/rados/RadosObject.h"

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

RadosObject::RadosObject(Stream& s) {
    s >> pool_;
    s >> oid_;
}


void RadosObject::encode(Stream& s) const {
    s << pool_;
    s << oid_;
}

RadosObject::RadosObject(const RadosObject& other, size_t part) {
    *this = other;
    if (part) {
        std::ostringstream oss;
        oss << oid_ << ";part-" << part;
        oid_ = oss.str();
    }
}


RadosObject::RadosObject(const std::string& path) {
    // static const std::string defaultRadosPool = Resource<std::string>("defaultRadosPool", "default");
    static const std::string defaultRadosPool = Resource<std::string>("defaultRadosPool", "cephfs_data");

    Tokenizer parse(":");

    std::vector<std::string> bits;
    parse(path, bits);

    ASSERT(bits.size() == 1 || bits.size() == 2);

    if (bits.size() == 1) {
        oid_  = path;
        pool_ = defaultRadosPool;
    }
    else {
        pool_ = bits[0];
        oid_  = bits[1];
    }
}

RadosObject::RadosObject(const std::string& pool, const std::string& oid) :
    pool_(pool), oid_(oid) {}

std::string RadosObject::str() const {
    return pool_ + ':' + oid_;
}


void RadosObject::print(std::ostream& s) const {
    s << "RadosObject[pool=" << pool_ << ",oid=" << oid_ << "]";
}


}  // namespace eckit
