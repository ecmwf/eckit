// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/rados/RadosPool.h"

#include <string>
#include <utility>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosName.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

RadosPool::RadosPool(const eckit::URI& uri) : RadosPool(rados::namesIn(uri, 1, "rados:<pool>").front()) {}

RadosPool::RadosPool(std::string pool) : pool_{std::move(pool)} {
    rados::checkName("pool", pool_);
}

eckit::URI RadosPool::uri() const {
    return eckit::URI{"rados", eckit::PathName(pool_)};
}

bool RadosPool::exists() const {
    return RadosCluster::instance().poolExists(pool_);
}

void RadosPool::create() const {
    RadosCluster::instance().createPool(pool_);
}

void RadosPool::ensureCreated() const {
    RadosCluster::instance().ensurePool(pool_);
}

void RadosPool::destroy() const {
    if (!RadosCluster::instance().destroyPoolIfExists(pool_)) {
        throw NotFound("RADOS pool " + pool_, Here(), true);
    }
}

void RadosPool::ensureDestroyed() const {
    RadosCluster::instance().destroyPoolIfExists(pool_);
}

std::vector<std::string> RadosPool::listNamespaces() const {
    return RadosCluster::instance().listNamespaces(pool_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
