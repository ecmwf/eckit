/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Log.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date May 1996

#ifndef eckit_config_Resource_h
#define eckit_config_Resource_h

#include <string>

#include "eckit/memory/NonCopyable.h"

namespace eckit {

class Configurable;
class Url;

class ResourceBase : private NonCopyable {
public:

// -- Contructors

    ResourceBase(Configurable* owner, const std::string& str);

// -- Destructor

    virtual ~ResourceBase();

// -- Methods

    void reset()            { inited_ = false;   }
    void dump(std::ostream&) const;
    void html(std::ostream&, Url&);

    std::string name() const;

protected:

// -- Methods

    void init();

private:

// -- Members

    bool           inited_;
    Configurable*  owner_;
    std::string         name_;        // In the config file
    std::string         environment_; // In the environment variables
    std::string         options_;     // For the command line options

// -- Methods

    virtual void setValue(const std::string&) = 0;
    virtual std::string getValue() const      = 0;

};




template<class T> class Resource : public ResourceBase {
public:

// -- Contructors

    // Standalone

    Resource(const std::string& str, const T& value):
        ResourceBase(0, str),     value_(value) {}

    // Part of a configurable

    Resource(Configurable* owner, const std::string& str, const T& value):
        ResourceBase(owner, str), value_(value) {}

// -- Convertors

    operator T&()                { init(); return value_;        }

private:

// -- Members

    T value_;

// -- Overridden methods

    // From ResourceBase

    virtual void setValue(const std::string&);
    virtual std::string getValue() const;

};

}

#include "eckit/config/Resource.cc"

#endif
