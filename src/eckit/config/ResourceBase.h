/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date May 1996

#pragma once
#include "eckit/config/Configurable.h"

#include <iosfwd>
#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class ResourceBase {

public:  // methods

    ResourceBase(Configurable* owner, const std::string& str);

    ResourceBase(const ResourceBase&)            = delete;
    ResourceBase& operator=(const ResourceBase&) = delete;
    ResourceBase(ResourceBase&&)                 = delete;
    ResourceBase& operator=(ResourceBase&&)      = delete;

    virtual ~ResourceBase();

    void reset() { inited_ = false; }
    void dump(std::ostream&) const;
    void GET(std::ostream&, Url&);

    std::string name() const;

protected:  // methods

    void init();

    virtual bool setFromConfigFile();

private:  // members

    Configurable* owner_;

    std::string name_;         // In the config file
    std::string environment_;  // In the environment variables
    std::string options_;      // For the command line options

    bool inited_;

private:  // methods

    virtual void setValue(const std::string&) = 0;
    virtual std::string getValue() const      = 0;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
