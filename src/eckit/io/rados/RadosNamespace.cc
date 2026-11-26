// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/rados/RadosNamespace.h"

#include <string>
#include <vector>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosObject.h"
#include "eckit/io/rados/RadosPool.h"
#include "eckit/log/Log.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

RadosNamespace::RadosNamespace(const eckit::URI& uri) {

    Tokenizer parse("/");

    std::vector<std::string> bits;
    parse(uri.name(), bits);

    if (bits.size() != 2) {
        throw eckit::UserError("Invalid RADOS URI '" + uri.asString() + "', expected rados:<pool>/<namespace>", Here());
    }

    pool_ = eckit::RadosPool(bits[0]);
    ns_   = bits[1];
    RadosPool::checkName("namespace", ns_);
}

RadosNamespace::RadosNamespace(const std::string& pool, const std::string& nspace) : pool_({pool}), ns_(nspace) {
    RadosPool::checkName("namespace", ns_);
}

eckit::URI RadosNamespace::uri() const {
    if (ns_.empty()) {
        throw eckit::UserError(
            "The namespace " + str() + " is in the default namespace, which a RADOS URI cannot express", Here());
    }
    return eckit::URI{"rados", eckit::PathName(str())};
}

std::string RadosNamespace::str() const {
    return pool_.name() + '/' + ns_;
}

std::vector<eckit::RadosObject> RadosNamespace::listObjects() const {

    std::vector<eckit::RadosObject> res;

    for (const auto& i : eckit::RadosCluster::instance().listObjects(pool_.name(), ns_)) {

        /// @note: an object written by another tool can have a name which cannot be addressed here
        if (!RadosPool::validName(i)) {
            LOG_DEBUG_LIB(LibEcKit) << "RadosNamespace::listObjects skipping object with unsupported name '" << i
                                    << "' in " << str() << std::endl;
            continue;
        }

        res.emplace_back(pool_.name(), ns_, i);
    }

    return res;
}

void RadosNamespace::destroy() const {
    for (auto& object : listObjects()) {
        object.ensureDestroyed();
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
