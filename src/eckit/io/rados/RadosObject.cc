// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

RadosObject::RadosObject(const std::string& pool, const std::string& oid) : pool_(pool), oid_(oid) {}

std::string RadosObject::str() const {
    return pool_ + ':' + oid_;
}


void RadosObject::print(std::ostream& s) const {
    s << "RadosObject[pool=" << pool_ << ",oid=" << oid_ << "]";
}


}  // namespace eckit
