/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/rados/RadosPool.h"
#include "eckit/io/rados/RadosNamespace.h"

// #include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

RadosPool::RadosPool(const eckit::URI& uri) {
    // static const std::string defaultRadosPool = Resource<std::string>("defaultRadosPool", "default");

    Tokenizer parse("/");

    std::vector<std::string> bits;
    parse(uri.name(), bits);

    ASSERT(bits.size() == 1);

    pool_ = bits[0];
}

RadosPool::RadosPool(const std::string& pool) : pool_(pool) {}

void RadosPool::destroy() const {
    /// @note: explicitly destroy every object in the pool, as rados_pool_delete
    ///   returns before the objects are destroyed, which is done in the background
    for (const auto& ns : listNamespaces()) {
        eckit::RadosNamespace(pool_, ns).destroy();
    }
    return RadosCluster::instance().destroyPool(pool_);
}

void RadosPool::ensureDestroyed() const {

    try {
        destroy();
    } catch (eckit::RadosEntityNotFoundException& e) {}

}

// std::string RadosObject::str() const {
//     return pool_ + ':' + oid_;
// }


// void RadosObject::print(std::ostream& s) const {
//     s << "RadosObject[pool=" << pool_ << ",oid=" << oid_ << "]";
// }

}  // namespace eckit
