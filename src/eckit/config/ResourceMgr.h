/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_ResourceMgr_h
#define eckit_ResourceMgr_h

#include "eckit/config/Script.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/types/Types.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

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

    /// prints the consolidated script
    /// @param out stream where to print
    void printScript( std::ostream& out );

    void appendConfig(std::istream& in);

    bool appendConfig( const PathName& path );

private: // methods

    /// private contructor for singleton
    ResourceMgr();

private: // members

    bool inited_;

    eckit::config::Script script_;
};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
