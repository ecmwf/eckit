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
#include "eckit/io/rados/RadosPool.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RadosNamespace {
public:

    explicit RadosNamespace(const eckit::URI& uri);
    RadosNamespace(const std::string& pool, const std::string& nspace);

    const eckit::RadosPool& pool() const { return pool_; }
    const std::string& name() const { return ns_; }
    /// @note: the default (empty) namespace cannot be expressed in a URI, so this throws for it
    eckit::URI uri() const;
    std::string str() const;

    bool exists() const { return pool_.exists(); };
    std::vector<eckit::RadosObject> listObjects() const;

    void destroy() const;

private:  // methods

    friend class RadosObject;
    friend class RadosKeyValue;
    RadosNamespace() = default;

private:  // members

    eckit::RadosPool pool_;
    std::string ns_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
