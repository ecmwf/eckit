/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/sql/SQLDatabase.h"

#include <memory>
#include <utility>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/sql/expression/SQLExpression.h"
#include "eckit/sql/type/SQLType.h"
#include "eckit/utils/Tokenizer.h"

using namespace eckit;

namespace eckit {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

SQLDatabase::SQLDatabase(const std::string& name) :
    name_(name) {}


SQLDatabase::~SQLDatabase() {}

void SQLDatabase::open() {
    Log::debug<LibEcKit>() << "Opening database: " << name_ << std::endl;
}

void SQLDatabase::close() {
    tablesByName_.clear();
}

void SQLDatabase::addTable(SQLTable* table) {
    tablesByName_.emplace(table->name(), std::unique_ptr<SQLTable>(table));
}

void SQLDatabase::addImplicitTable(SQLTable* table) {
    implicitTables_.emplace_back(table);
}

void SQLDatabase::setLinks(const Links& links) {
    for (Links::const_iterator j = links.begin(); j != links.end(); ++j) {
        const std::string& from         = (*j).first;
        const std::set<std::string>& to = (*j).second;

        ASSERT(tablesByName_.find(from) != tablesByName_.end());
        SQLTable& f = *tablesByName_[from];

        for (std::set<std::string>::const_iterator k = to.begin(); k != to.end(); ++k) {
            ASSERT(tablesByName_.find(*k) != tablesByName_.end());
            SQLTable& t = *tablesByName_[*k];

            f.addLinkTo(t);
            t.addLinkFrom(f);
        }
    }
}


SQLTable& SQLDatabase::defaultTable() {
    auto it = tablesByName_.find("defaultTable");
    if (it == tablesByName_.end()) {
        if (tablesByName_.empty() && implicitTables_.size() != 0) {
            return *implicitTables_.back();
        }
        throw UserError("No default table");
    }
    return *(it->second);
}

std::vector<std::reference_wrapper<SQLTable>> SQLDatabase::implicitTables() {
    std::vector<std::reference_wrapper<SQLTable>> tables;
    for (std::unique_ptr<SQLTable>& ptable : implicitTables_) {
        ASSERT(ptable);
        tables.push_back(*ptable);
    }
    return tables;
}

bool SQLDatabase::hasTable(const std::string& name) const {
    return tablesByName_.find(name) != tablesByName_.end();
}

SQLTable& SQLDatabase::table(const std::string& nm) {
    auto it = tablesByName_.find(nm);
    ASSERT(it != tablesByName_.end());
    return *(it->second);
}

void SQLDatabase::setVariable(const std::string& name, std::shared_ptr<expression::SQLExpression> value) {
    variables_[name] = value;
}

std::shared_ptr<expression::SQLExpression> SQLDatabase::getVariable(const std::string& name) const {
    Variables::const_iterator j = variables_.find(name);
    if (j == variables_.end())
        throw eckit::UserError("Undefined variable", name);
    return j->second;
}

void SQLDatabase::setIncludePath(const std::string& includePath) {
    Tokenizer tokenize(":");
    std::vector<std::string> tokens;
    tokenize(includePath, tokens);
    copy(tokens.begin(), tokens.end(), back_inserter(includePath_));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace sql
}  // namespace eckit
