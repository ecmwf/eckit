// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Nicolau Manubens
/// @author Metin Çakırcalı
/// @date February 2024

#pragma once

#include <string>
#include <vector>

#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/URI.h"
#include "eckit/io/rados/RadosCluster.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RadosPool {
public:

    /// Checks that a pool, namespace or object name is one which can be addressed.
    /// A name with '/', '#' or '?', or equal to "." or "..", does not survive in a URI, where it would
    /// silently name something else, and one with a NUL byte is cut short by librados.
    /// @param what describes the name in the error, e.g. "pool"
    /// @throws UserError
    static void checkName(const std::string& what, const std::string& name);

    /// @returns whether checkName() accepts the name
    static bool validName(const std::string& name);

    explicit RadosPool(const eckit::URI& uri);
    explicit RadosPool(std::string pool);

    const std::string& name() const { return pool_; }
    eckit::URI uri() const { return eckit::URI{"rados", eckit::PathName(str())}; }
    std::string str() const { return name(); };
    bool exists() const { return RadosCluster::instance().poolExists(pool_); }

    void create() const { RadosCluster::instance().createPool(pool_); }
    void ensureCreated() const { RadosCluster::instance().ensurePool(pool_); }
    void destroy() const;
    void ensureDestroyed() const;
    std::vector<std::string> listNamespaces() const { return RadosCluster::instance().listNamespaces(pool_); }

private:  // methods

    friend class RadosNamespace;
    RadosPool() {}

private:  // members

    std::string pool_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
