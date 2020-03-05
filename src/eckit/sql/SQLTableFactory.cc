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

SQLTableFactory& SQLTableFactory::instance() {
    static SQLTableFactory theInstance;
    return theInstance;
}

SQLTable* SQLTableFactory::build(SQLDatabase& owner, const std::string& name, const std::string& location) {

    std::string location2 = location.size() ? location : name;

    // This sub-scope looks to be entirely superflouse.
    // See ECKIT-473
    // In this case, the cray compiler (8.6, 8.7) are optimising out the call of
    // std::~lock_guard() in the exception case if no factory succeeds.
    // This leads to the mutex still being held - at best resulting in an infinite hang
    // on destruction of global objects at the end of execution.
    //
    // The extra scope ensures that the mutex destructor is called _before_ the throw
    // statement.

    {
        std::lock_guard<std::mutex> lock(mutex_);

        for (const auto& factory : factories_) {
            SQLTable* t = factory->build(owner, name, location2);
            if (t)
                return t;
        }
    }

    throw UserError("No SQL table could be built for " + name + " (" + location2 + ")", Here());
}

void SQLTableFactory::enregister(SQLTableFactoryBase* f) {

    std::lock_guard<std::mutex> lock(mutex_);

    ASSERT(f);
    ASSERT(std::find(factories_.begin(), factories_.end(), f) == factories_.end());
    factories_.push_back(f);
}

void SQLTableFactory::deregister(SQLTableFactoryBase* f) {

    std::lock_guard<std::mutex> lock(mutex_);

    auto it = std::find(factories_.begin(), factories_.end(), f);
    ASSERT(it != factories_.end());
    factories_.erase(it);
}

//----------------------------------------------------------------------------------------------------------------------

SQLTableFactoryBase::SQLTableFactoryBase() {
    SQLTableFactory::instance().enregister(this);
}


SQLTableFactoryBase::~SQLTableFactoryBase() {
    SQLTableFactory::instance().deregister(this);
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace sql
}  // namespace eckit
