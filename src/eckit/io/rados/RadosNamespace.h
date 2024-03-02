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

#include "eckit/io/rados/RadosPool.h"
#include "eckit/io/rados/RadosCluster.h"

namespace eckit {

class RadosNamespace {
public:

    RadosNamespace(const eckit::URI&);
    RadosNamespace(const std::string& pool, const std::string& nspace);

    const eckit::RadosPool& pool() const { return pool_; }
    const std::string& name() const { return ns_; }
    eckit::URI uri() const { return eckit::URI{"rados", eckit::PathName(str())}; }
    std::string str() const;
    
    bool exists() const { return pool_.exists(); };
    std::vector<eckit::RadosObject> listObjects() const;

    void destroy();

private:  // methods
    friend class RadosObject;
    RadosNamespace() {}

private:  // members
    eckit::RadosPool pool_;
    std::string ns_;

};


}  // namespace eckit
