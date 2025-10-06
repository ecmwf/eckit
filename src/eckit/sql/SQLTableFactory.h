/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @date Aug 2018

#ifndef eckit_sql_SQLTableFactory_H
#define eckit_sql_SQLTableFactory_H

#include <mutex>
#include <string>
#include <vector>

/// Depending on what is being used, we need to be able to open SQL tables of different
/// sorts from different locations.
///
/// --> The SQLParser/SQLSession can see a select statement such as "select a from b.tbl"
///     and look for the handler of the file b.tbl
///
/// TODO: Consider whether this really ought to be a global factory, or something that
///       is handled on a per session basis.

namespace eckit::sql {

class SQLTable;
class SQLTableFactoryBase;
class SQLDatabase;

//----------------------------------------------------------------------------------------------------------------------

class SQLTableFactory {

    // n.b. we currently have no use for a name, but we may well in the future.
    // typedef std::map<std::string, const SQLTableFactoryBase*> factory_map;
    typedef std::vector<const SQLTableFactoryBase*> factory_map;

private:  // methods

    // Can only be constructed by instance()
    SQLTableFactory()  = default;
    ~SQLTableFactory() = default;

public:  // methods

    static SQLTableFactory& instance();

    /// Build an SQLTable from a give name. Location is optional - if it is not supplied
    /// then the name may be treated as a location at the discretion of the handler.

    SQLTable* build(SQLDatabase& owner, const std::string& name, const std::string& location = "");

    void enregister(SQLTableFactoryBase* f);
    void deregister(SQLTableFactoryBase* f);

private:  // methods

    factory_map factories_;
    std::mutex mutex_;
};

//----------------------------------------------------------------------------------------------------------------------

class SQLTableFactoryBase {

public:  // methods

    SQLTableFactoryBase();
    virtual ~SQLTableFactoryBase();

    virtual SQLTable* build(SQLDatabase& owner, const std::string& name, const std::string& location) const = 0;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::sql

#endif
