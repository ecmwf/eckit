/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/SQLTableFactory.h"

#include <algorithm>

#include "eckit/exception/Exceptions.h"


namespace eckit {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

SQLTable* SQLTableFactory::build(SQLDatabase& owner, const std::string& name, const std::string& location) {

    std::string location2 = location.size() ? location : name;

    std::lock_guard<std::mutex> lock(mutex());

    for (const auto& factory : factories()) {
        SQLTable* t = factory->build(owner, name, location2);
        if (t)
            return t;
    }

    throw UserError("No SQL table could be built for " + name + " (" + location2 + ")", Here());
}

void SQLTableFactory::registration(SQLTableFactoryBase* f) {

    std::lock_guard<std::mutex> lock(mutex());

    ASSERT(f);
    ASSERT(std::find(factories().begin(), factories().end(), f) == factories().end());
    factories().push_back(f);
}

void SQLTableFactory::deregister(SQLTableFactoryBase* f) {

    std::lock_guard<std::mutex> lock(mutex());

    auto it = std::find(factories().begin(), factories().end(), f);
    ASSERT(it != factories().end());
    factories().erase(it);
}

SQLTableFactory::factory_map& SQLTableFactory::factories() {
    static factory_map m;
    return m;
}

std::mutex& SQLTableFactory::mutex() {
    static std::mutex m;
    return m;
}

//----------------------------------------------------------------------------------------------------------------------

SQLTableFactoryBase::SQLTableFactoryBase() {
    SQLTableFactory::registration(this);
}


SQLTableFactoryBase::~SQLTableFactoryBase() {
    SQLTableFactory::deregister(this);
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace sql
}  // namespace eckit
