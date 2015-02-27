/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File ResourceMgr.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_ResourceMgr_h
#define eckit_ResourceMgr_h

#include <set>

#include "eckit/memory/NonCopyable.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/types/Types.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class LocalPathName;
class Configurable;

namespace config { class Script; }

/// ResourceMgr is a singleton class
class ResourceMgr : private NonCopyable {

public: // methods

    /// destructor for singleton
    ~ResourceMgr();

    /// @returns the singleton instance of this class
    static ResourceMgr& instance();

    /// @brief Clear all stored Resources
    void reset();

    /// @brief Lookup Resource value
    bool lookUp( Configurable*, const std::string&, const StringDict* args, std::string&);

    /// @brief Set a Resource
    void set(const std::string&,const std::string&);

    /// @brief Append configuration stream for Resource lookup
    void appendConfig( std::istream& in );

    /// @brief Append configuration file for Resource lookup.
    /// @returns true if appended
    /// Internally the ContextBehavior will be responsible for the actual reading.
    bool appendConfig( const PathName& );

    /// prints the consolidated script
    /// @param out stream where to print
    void printScript( std::ostream& out );

protected: // methods

    void readConfigFiles();

private: // methods

    /// private contructor for singleton
    ResourceMgr();

private: // members

    bool inited_;

    config::Script* script_;

    std::set<PathName> parsed_;
};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
