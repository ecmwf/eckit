// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Nicolau Manubens
/// @author Metin Çakırcalı
/// @date February 2024

#pragma once

#include <string>
#include <vector>

#include "eckit/filesystem/URI.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RadosPool {
public:

    explicit RadosPool(const eckit::URI& uri);
    explicit RadosPool(std::string pool);

    const std::string& name() const { return pool_; }
    eckit::URI uri() const;
    std::string str() const { return pool_; }

    bool exists() const;

    /// @throws AlreadyExists
    void create() const;
    void ensureCreated() const;

    /// Removes the pool and everything in it
    /// @throws NotFound
    void destroy() const;
    void ensureDestroyed() const;

    /// @note: goes through every object of the pool, as Ceph keeps no list of namespaces
    std::vector<std::string> listNamespaces() const;

private:  // members

    std::string pool_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
