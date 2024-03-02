/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Nicolau Manubens
/// @date February 2024

#pragma once

// #include <memory>
// #include <string>

#include "eckit/filesystem/URI.h"
#include "eckit/io/rados/RadosCluster.h"

namespace eckit {

class RadosPool {
public:
    RadosPool(const eckit::URI&);
    RadosPool(const std::string& pool);

    const std::string& name() const { return pool_; }
    eckit::URI uri() const { return eckit::URI{"rados", eckit::PathName(str())}; }
    std::string str() const { return name(); };
    bool exists() const { return RadosCluster::instance().poolExists(pool_); }

    void create() const { return RadosCluster::instance().createPool(pool_); }
    void ensureCreated() const { return RadosCluster::instance().ensurePool(pool_); }
    void destroy() const { return RadosCluster::instance().destroyPool(pool_); }
    void ensureDestroyed() const;

private:  // methods
    friend class RadosNamespace;
    RadosPool() {}

private: // members
    std::string pool_;
};


}  // namespace eckit
