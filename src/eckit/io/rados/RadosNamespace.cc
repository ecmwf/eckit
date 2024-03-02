/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/rados/RadosNamespace.h"
#include "eckit/io/rados/RadosObject.h"

// #include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

RadosNamespace::RadosNamespace(const eckit::URI& uri) {

    Tokenizer parse("/");

    std::vector<std::string> bits;
    parse(uri.name(), bits);

    ASSERT(bits.size() == 2);

    pool_ = eckit::RadosPool(bits[0]);
    ns_ = bits[1];

}

RadosNamespace::RadosNamespace(const std::string& pool, const std::string& nspace) : pool_({pool}), ns_(nspace) {}

// std::string RadosObject::str() const {
//     return pool_ + ':' + oid_;
// }


std::string RadosNamespace::str() const {
    return pool_.name() + '/' + ns_;
}

std::vector<eckit::RadosObject> RadosNamespace::listObjects() const {

    std::vector<eckit::RadosObject> res;

    for (const auto& i : eckit::RadosCluster::instance().listObjects(pool_.name(), ns_))
        res.push_back(eckit::RadosObject(pool_.name(), ns_, i));

    return res;

}

void RadosNamespace::destroy() {

    for (auto& i : listObjects())
        i.ensureDestroyed();

}

// void RadosObject::print(std::ostream& s) const {
//     s << "RadosObject[pool=" << pool_ << ",oid=" << oid_ << "]";
// }

}  // namespace eckit
