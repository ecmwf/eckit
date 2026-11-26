// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/rados/RadosNamespace.h"

#include <ostream>
#include <string>
#include <vector>

#include "eckit/config/LibEcKit.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosName.h"
#include "eckit/io/rados/RadosObject.h"
#include "eckit/io/rados/RadosPool.h"
#include "eckit/log/Log.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

RadosNamespace::RadosNamespace(const eckit::URI& uri) :
    RadosNamespace(rados::namesIn(uri, 2, "rados:<pool>/<namespace>")) {}

RadosNamespace::RadosNamespace(const std::vector<std::string>& names) : RadosNamespace(names[0], names[1]) {}

RadosNamespace::RadosNamespace(const std::string& pool, const std::string& nspace) : pool_(pool), ns_(nspace) {
    rados::checkName("namespace", ns_);
}

eckit::URI RadosNamespace::uri() const {
    return rados::uriOf(str(), ns_);
}

std::string RadosNamespace::str() const {
    return pool_.name() + '/' + ns_;
}

std::vector<eckit::RadosObject> RadosNamespace::listObjects() const {

    std::vector<eckit::RadosObject> res;

    for (const auto& name : eckit::RadosCluster::instance().listObjects(pool_.name(), ns_)) {

        /// @note: an object written by another tool can have a name which cannot be addressed here
        if (!rados::validName(name)) {
            LOG_DEBUG_LIB(LibEcKit) << "RadosNamespace::listObjects skipping object with unsupported name '" << name
                                    << "' in " << str() << std::endl;
            continue;
        }

        res.emplace_back(pool_.name(), ns_, name);
    }

    return res;
}

void RadosNamespace::destroy() const {
    for (const auto& object : listObjects()) {
        object.ensureDestroyed();
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
