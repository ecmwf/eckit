// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date May 1996

#ifndef eckit_config_ResourceBase_h
#define eckit_config_ResourceBase_h

#include <string>

#include "eckit/utils/Tokenizer.h"
#include "eckit/utils/Translator.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Configurable;
class Url;

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

#endif
