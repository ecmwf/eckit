/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date Apr 2015

#ifndef eckit_config_Parametrisation_H
#define eckit_config_Parametrisation_H

#include <string>
#include <vector>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Parametrisation {

public: // methods

    /// Destructor redundant but fixes sanity compiler warnings
    virtual ~Parametrisation() {}

    virtual bool has(const std::string& name) const = 0;

    virtual bool get(const std::string& name, std::string& value) const = 0;
    virtual bool get(const std::string& name, bool& value) const = 0;
    virtual bool get(const std::string& name, long& value) const = 0;
    virtual bool get(const std::string& name, size_t& value) const = 0;
    virtual bool get(const std::string& name, double& value) const = 0;

    virtual bool get(const std::string& name, std::vector<long>& value) const = 0;
    virtual bool get(const std::string& name, std::vector<double>& value) const = 0;

};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif

