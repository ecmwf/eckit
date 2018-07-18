/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File SQLInsert.h
// Piotr Kuchta - ECMWF July 2016

#ifndef SQLInsertFactory_H
#define SQLInsertFactory_H

#include "eckit/thread/ThreadSingleton.h"
#include "odb_api/Expressions.h"
#include "SQLOutputConfig.h"
#include "odb_api/SchemaAnalyzer.h"
#include "SQLAST.h"
#include "SQLInsert.h"

namespace eckit { class DataHandle; }
namespace eckit { namespace sql { class DataTable; } }
namespace eckit { namespace sql { class SQLDatabase; } }

namespace eckit { 
namespace sql {

class SQLSession;

class SQLInsertFactory {
public:
    SQLInsertFactory();

	SQLInsert* create(odb::sql::SQLSession& session, const InsertAST&);

private:
    // No copy allowed
    SQLInsertFactory(const SQLInsertFactory&);
    SQLInsertFactory& operator=(const SQLInsertFactory&);
};

} // namespace sql
} // namespace eckit

#endif
