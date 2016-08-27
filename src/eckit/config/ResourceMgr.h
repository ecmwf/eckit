/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ResourceMgr.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_config_ResourceMgr_H
#define eckit_config_ResourceMgr_H

namespace eckit {

class LocalPathName;

class ResourceMgr {
public:

// -- Class methods

    static void reset();
    static bool lookUp(const std::string&, const std::string&, const std::string&, std::string&);
    static void set(const std::string&, const std::string&);

private:

    ResourceMgr();

// No copy allowed

    ResourceMgr(const ResourceMgr&);
    ResourceMgr& operator=(const ResourceMgr&);

// -- Members

    static bool inited_;

// -- Methods

    static void readConfigFile(const LocalPathName&);
    static bool parse(const char*);

};

// A resource specifier

class ResourceQualifier {

    std::string kind_;  // Kind,  e.g. "Application"
    std::string owner_; // Owner, e.g. "mars"
    std::string name_;  // Name,  e.g. "debug"

public:

    ResourceQualifier();
    ResourceQualifier(const std::string&, const std::string&, const std::string&);

    ResourceQualifier(const ResourceQualifier&);
    ResourceQualifier& operator=(const ResourceQualifier&);

    int operator<(const ResourceQualifier&) const;

};
}
#endif
