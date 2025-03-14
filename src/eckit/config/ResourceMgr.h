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
/// @date   May 96

#ifndef eckit_config_ResourceMgr_H
#define eckit_config_ResourceMgr_H

#include <map>
#include <string>

#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/Mutex.h"

namespace eckit {

class LocalPathName;


//----------------------------------------------------------------------------------------------------------------------

// A resource specifier

class ResourceQualifier {

    std::string kind_;   // Kind,  e.g. "Application"
    std::string owner_;  // Owner, e.g. "mars"
    std::string name_;   // Name,  e.g. "debug"

public:

    ResourceQualifier();
    ResourceQualifier(const std::string&, const std::string&, const std::string&);

    ResourceQualifier(const ResourceQualifier&);
    ResourceQualifier& operator=(const ResourceQualifier&);

    int operator<(const ResourceQualifier&) const;
};

//----------------------------------------------------------------------------------------------------------------------


class ResourceMgr : private eckit::NonCopyable {

public:  // class methods

    static ResourceMgr& instance();

    static bool lookUp(const std::string&, const std::string&, const std::string&, std::string&);

    bool registCmdArgOptions(const std::string&);

private:

    ResourceMgr();

    bool doLookUp(const std::string&, const std::string&, const std::string&, std::string&);

    // Only for my friends
    // You should never call these, resources have a read-only semantics
    void reset();
    void set(const std::string&, const std::string&);

    friend class ConfigCmd;
    friend class ResourceBase;

private:  // methods

    void readConfigFile(const LocalPathName&);
    bool parse(const char*);

private:  // members

    typedef std::map<ResourceQualifier, std::string> ResMap;

    ResMap resmap_;

    std::map<std::string, std::string> resoptions_;

    Mutex mutex_;

    bool inited_;
};

//----------------------------------------------------------------------------------------------------------------------


}  // namespace eckit

#endif
