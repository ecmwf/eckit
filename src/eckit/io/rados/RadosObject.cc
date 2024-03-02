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
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosHandle.h"
#include "eckit/io/rados/RadosPersistentHandle.h"
#include "eckit/io/rados/RadosMultiObjWriteHandle.h"
#include "eckit/io/rados/RadosMultiObjReadHandle.h"
#include "eckit/io/PartHandle.h"

// #include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

// RadosObject::RadosObject(Stream& s) {
//     s >> pool_;
//     s >> oid_;
// }


// void RadosObject::encode(Stream& s) const {
//     s << pool_;
//     s << oid_;
// }

RadosObject::RadosObject(const RadosObject& other, size_t part) {
    *this = other;
    if (part) {
        std::ostringstream oss;
        oss << oid_ << ";part-" << part;
        oid_ = oss.str();
    }
}


RadosObject::RadosObject(const eckit::URI& uri) {

    Tokenizer parse("/");

    std::vector<std::string> bits;
    parse(uri.name(), bits);

    ASSERT(bits.size() == 3);

    ns_ = eckit::RadosNamespace{bits[0], bits[1]};
    oid_  = bits[2];

}

RadosObject::RadosObject(const std::string& pool, const std::string& nspace, const std::string& oid) :
    ns_(pool, nspace), oid_(oid) {}

std::string RadosObject::str() const {
    return ns_.pool().name() + '/' + ns_.name() + '/' + oid_;
}

bool RadosObject::exists() const {

    return eckit::RadosCluster::instance().exists(*this);

}

void RadosObject::ensureDestroyed() {

    try {

        eckit::RadosCluster::instance().remove(*this);

    } catch (eckit::SeriousBug& e) {}

}

void RadosObject::ensureAllDestroyed() {

    try {

        eckit::RadosCluster::instance().removeAll(*this);

    } catch (eckit::SeriousBug& e) {}

}

eckit::DataHandle* RadosObject::dataHandle() const {

    return new eckit::RadosHandle(*this);

}

eckit::DataHandle* RadosObject::persistentDataHandle(bool persist_on_write, size_t maxAioBuffSize) const {

    return new eckit::RadosPersistentHandle(*this, persist_on_write, maxAioBuffSize);

}

eckit::DataHandle* RadosObject::rangeReadHandle(const eckit::Offset& off, const eckit::Length& len) const {

    return new eckit::PartHandle(new eckit::RadosHandle(*this), off, len);

}

eckit::DataHandle* RadosObject::multipartWriteHandle(const eckit::Length& maxObjectSize) const {

    return new eckit::RadosMultiObjWriteHandle(*this, false, maxObjectSize);

}

eckit::DataHandle* RadosObject::persistentMultipartWriteHandle(const eckit::Length& maxObjectSize, 
    size_t maxAioBuffSize, size_t maxHandleBuffSize) const {

    return new eckit::RadosMultiObjWriteHandle(*this, true, maxObjectSize, maxAioBuffSize, maxHandleBuffSize);

}

eckit::DataHandle* RadosObject::multipartRangeReadHandle(const eckit::Offset& off, const eckit::Length& len) const {

    return new eckit::PartHandle(new eckit::RadosMultiObjReadHandle(*this), off, len);

}

// void RadosObject::print(std::ostream& s) const {
//     s << "RadosObject[pool=" << pool_ << ",oid=" << oid_ << "]";
// }


}  // namespace eckit
