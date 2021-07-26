/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/type/SQLType.h"

#include <map>
#include <memory>
#include <mutex>

#include "eckit/exception/Exceptions.h"
#include "eckit/log/Log.h"
#include "eckit/sql/type/SQLDouble.h"
#include "eckit/sql/type/SQLInt.h"
#include "eckit/sql/type/SQLReal.h"
#include "eckit/sql/type/SQLString.h"
#include "eckit/thread/ThreadSingleton.h"
#include "eckit/utils/Translator.h"

using namespace eckit;

namespace eckit {
namespace sql {
namespace type {

//----------------------------------------------------------------------------------------------------------------------

class TypeRegistry {
public:
    TypeRegistry();
    ~TypeRegistry() = default;

    static TypeRegistry& instance();

    // enregister takes ownership
    void enregister(SQLType* t);
    void registerAlias(const std::string& name, const std::string& alias);
    const SQLType* lookup(const std::string& name);

private:
    void enregisterInternal(SQLType* t);
    std::mutex m_;
    std::map<std::string, std::unique_ptr<SQLType>> map_;
    std::map<std::string, const SQLType*> aliases_;
};

//----------------------------------------------------------------------------------------------------------------------

TypeRegistry::TypeRegistry() {
    enregisterInternal(new SQLInt("integer"));
    enregisterInternal(new SQLReal("real"));
    enregisterInternal(new SQLDouble("double"));
}

TypeRegistry& TypeRegistry::instance() {
    static TypeRegistry theRegistry;
    return theRegistry;
}

void TypeRegistry::enregister(SQLType* t) {
    std::lock_guard<std::mutex> lock(m_);
    enregisterInternal(t);
}

void TypeRegistry::enregisterInternal(SQLType* t) {
    map_.emplace(std::make_pair(t->name(), std::unique_ptr<SQLType>(t)));
}

void TypeRegistry::registerAlias(const std::string& name, const std::string& alias) {
    std::lock_guard<std::mutex> lock(m_);
    auto it = map_.find(name);
    ASSERT(it != map_.end());
    aliases_.emplace(std::make_pair(alias, it->second.get()));
}

const SQLType* TypeRegistry::lookup(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_);

    auto it = map_.find(name);
    if (it != map_.end())
        return it->second.get();

    auto it2 = aliases_.find(name);
    if (it2 != aliases_.end())
        return it2->second;

    return nullptr;
}

SQLType::SQLType(const std::string& name) :
    name_(name) {}

SQLType::~SQLType() {}


size_t SQLType::width() const {
    return 14;
}

SQLType::manipulator SQLType::format() const {
    return &std::right;
}

bool SQLType::exists(const std::string& name) {
    const SQLType* typ = TypeRegistry::instance().lookup(name);
    return !!typ;
}

const SQLType& SQLType::lookup(const std::string& name, size_t sizeDoubles) {

    std::string lookupName = name;
    if (name == "string") {
        lookupName += Translator<size_t, std::string>()(sizeof(double) * sizeDoubles);
    }
    else {
        ASSERT(sizeDoubles == 1);
    }

    const SQLType* typ = TypeRegistry::instance().lookup(lookupName);

    if (!typ && name == "string") {
        typ = SQLType::registerType(new SQLString(lookupName, sizeof(double) * sizeDoubles));
    }

    if (!typ)
        throw eckit::SeriousBug(name + ": type not defined");
    return *typ;
}

void SQLType::createAlias(const std::string& name, const std::string& alias) {
    TypeRegistry::instance().registerAlias(name, alias);
}

SQLType* SQLType::registerType(SQLType* t) {
    TypeRegistry::instance().enregister(t);
    return t;
}

void SQLType::print(std::ostream& s) const {
    s << name_;
}

const SQLType* SQLType::subType(const std::string&) const {
    return this;
}


}  // namespace type
}  // namespace sql
}  // namespace eckit
