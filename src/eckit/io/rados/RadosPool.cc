// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/io/rados/RadosPool.h"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosException.h"
#include "eckit/log/Log.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

void RadosPool::checkName(const std::string& what, const std::string& name) {

    static const std::string forbidden("/#?\0", 4);

    if (name == "." || name == ".." || name.find_first_of(forbidden) != std::string::npos) {

        /// @note: a NUL byte would end the message early
        std::string printable = name;
        std::replace(printable.begin(), printable.end(), '\0', ' ');

        throw eckit::UserError("Invalid RADOS " + what + " name '" + printable +
                                   "': it must not contain '/', '#', '?' or a NUL byte, nor be '.' or '..'",
                               Here());
    }
}

RadosPool::RadosPool(const eckit::URI& uri) {
    Tokenizer parse("/");

    std::vector<std::string> bits;
    parse(uri.name(), bits);

    if (bits.size() != 1) {
        throw eckit::UserError("Invalid RADOS URI '" + uri.asString() + "', expected rados:<pool>", Here());
    }

    pool_ = bits[0];
    checkName("pool", pool_);
}

RadosPool::RadosPool(std::string pool) : pool_{std::move(pool)} {
    checkName("pool", pool_);
}

void RadosPool::destroy() const {
    /// @note: deleting a pool removes all its objects
    RadosCluster::instance().destroyPool(pool_);
}

void RadosPool::ensureDestroyed() const {
    RadosCluster::instance().destroyPoolIfExists(pool_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
