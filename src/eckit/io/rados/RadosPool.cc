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

#include <string>
#include <utility>
#include <vector>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosException.h"
#include "eckit/io/rados/RadosNamespace.h"
#include "eckit/log/Log.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

RadosPool::RadosPool(const eckit::URI& uri) {
    Tokenizer parse("/");

    std::vector<std::string> bits;
    parse(uri.name(), bits);

    ASSERT(bits.size() == 1);

    pool_ = bits[0];
}

RadosPool::RadosPool(std::string pool) : pool_{std::move(pool)} {}

void RadosPool::destroy() const {
    for (const auto& ns : listNamespaces()) {
        eckit::RadosNamespace(pool_, ns).destroy();
    }
    RadosCluster::instance().destroyPool(pool_);
}

void RadosPool::ensureDestroyed() const {
    try {
        destroy();
    }
    catch (eckit::RadosEntityNotFoundException& e) {
        LOG_DEBUG_LIB(LibEcKit) << e.what() << '\n';
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
