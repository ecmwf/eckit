/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Piotr Kuchta
/// @date June 2015

#ifndef eckit_config_DynamicParametrisation_H
#define eckit_config_DynamicParametrisation_H

#include "eckit/config/Parametrisation.h"

#include <string>
#include <vector>

namespace eckit {

class ExecutionContext;

class DynamicParametrisation : public eckit::Parametrisation {
public:

    DynamicParametrisation(eckit::ExecutionContext&);
    ~DynamicParametrisation();

    virtual bool has(const std::string& name) const;

    virtual bool get(const std::string& name, std::string& value) const;
    virtual bool get(const std::string& name, bool& value) const;
    virtual bool get(const std::string& name, long& value) const;
    virtual bool get(const std::string& name, size_t& value) const;
    virtual bool get(const std::string& name, double& value) const;

    virtual bool get(const std::string& name, std::vector<long>& value) const;
    virtual bool get(const std::string& name, std::vector<double>& value) const;

private:
    eckit::ExecutionContext& context_;
};

}  // namespace eckit

#endif

