// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Nicolau Manubens
/// @author Metin Çakırcalı
/// @date February 2024

#pragma once

#include <string>
#include <vector>

#include "eckit/filesystem/URI.h"
#include "eckit/io/rados/RadosPool.h"

namespace eckit {

class RadosObject;

//----------------------------------------------------------------------------------------------------------------------

/// A namespace of a pool. The empty name is the default namespace.
/// @note: Ceph has no namespaces as such, only objects in them, so a namespace neither exists nor is created.
class RadosNamespace {
public:

    explicit RadosNamespace(const eckit::URI& uri);
    RadosNamespace(const std::string& pool, const std::string& nspace);

    const eckit::RadosPool& pool() const { return pool_; }
    const std::string& name() const { return ns_; }

    /// @throws UserError for the default namespace, which a URI cannot express
    eckit::URI uri() const;
    std::string str() const;

    /// @note: objects whose name cannot be addressed (see rados::validName) are left out
    std::vector<eckit::RadosObject> listObjects() const;

    /// Removes the objects which listObjects() returns
    void destroy() const;

private:  // methods

    explicit RadosNamespace(const std::vector<std::string>& names);

private:  // members

    eckit::RadosPool pool_;
    std::string ns_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
