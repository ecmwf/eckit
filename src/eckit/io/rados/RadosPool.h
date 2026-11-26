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
