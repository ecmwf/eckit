/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/exception/Exceptions.h"
#include "eckit/thread/ThreadSingleton.h"
#include "eckit/log/Log.h"
#include "eckit/sql/type/SQLDouble.h"
#include "eckit/sql/type/SQLInt.h"
#include "eckit/sql/type/SQLReal.h"
#include "eckit/sql/type/SQLString.h"
#include "eckit/sql/type/SQLType.h"

using namespace eckit;

namespace eckit {
namespace sql {
namespace type {

class TypeRegistry {
public:
    TypeRegistry();
    static std::map<std::string,SQLType*>& typeMap();
    std::map<std::string,SQLType*>& map();

private:
    std::map<std::string,SQLType*> map_;

    SQLType* registerType(SQLType*);
};

static eckit::ThreadSingleton<TypeRegistry> typeRegistry_;

TypeRegistry::TypeRegistry()
: map_()
{
    registerType(new SQLInt("integer"));
    registerType(new SQLReal("real"));
    registerType(new SQLString("string"));
    registerType(new SQLDouble("double"));
}

SQLType* TypeRegistry::registerType(SQLType* t)
{
    map_.insert(make_pair(t->name(), t));
    return t;
}

std::map<std::string,SQLType*>& TypeRegistry::typeMap() { return typeRegistry_.instance().map(); }
std::map<std::string,SQLType*>& TypeRegistry::map() { return map_; }

SQLType* SQLType::registerType(SQLType* t)
{
    std::map<std::string,SQLType*>& map (TypeRegistry::typeMap());
    //ASSERT(map.find(t->name()) == map.end());
    map.insert(make_pair(t->name(), t));
    return t;
}

size_t SQLType::width() const { return 14; }
SQLType::manipulator SQLType::format() const { return &std::right; }

SQLType::SQLType(const std::string& name): name_(name) {}

SQLType::SQLType(const std::string& name, const std::string& shortName)
: name_(name)
{
    std::map<std::string,SQLType*>& map (typeRegistry_.instance().typeMap());
    ASSERT(map.find(name) == map.end());
    map[shortName] = map[name_] = this;
}

SQLType::~SQLType() {}

bool SQLType::exists(const std::string& name)
{
    std::map<std::string,SQLType*>& map (TypeRegistry::typeMap());
    return map.find(name) != map.end();
}

const SQLType& SQLType::lookup(const std::string& name)
{
    std::map<std::string,SQLType*>& map (TypeRegistry::typeMap());
    std::map<std::string,SQLType*>::iterator j (map.find(name));
    if(j == map.end())
        throw eckit::SeriousBug(name + ": type not defined");
    return *(*j).second;
}

void SQLType::createAlias(const std::string& name, const std::string& alias)
{
    std::map<std::string,SQLType*>& map (TypeRegistry::typeMap());
    ASSERT(SQLType::exists(name));
    map[alias] = map[name];
}

void SQLType::print(std::ostream& s) const { s << name_; }

const SQLType* SQLType::subType(const std::string&) const { return this; }


} // namespace type
} // namespace sql
} // namespace eckit

