/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef odb_api_SQLSelectFactory_H
#define odb_api_SQLSelectFactory_H

#include "odb_api/Expressions.h"
#include "odb_api/SchemaAnalyzer.h"
#include "eckit/sql/SQLAST.h"
#include "eckit/sql/SQLOutputConfig.h"

namespace eckit { class DataHandle; }
namespace eckit { namespace sql { class DataTable; } }
namespace eckit { namespace sql { class SQLDatabase; } }
namespace eckit { namespace sql { class SQLSession; } }

namespace eckit { 
namespace sql {

class SQLSelectFactory {
public:
    SQLSelectFactory(const SQLOutputConfig&, const std::string&);

	SQLSelect* create(odb::sql::SQLSession&, const SelectAST&);

	SQLSelect* create(
        odb::sql::SQLSession&,
        bool distinct,
        bool all,
        Expressions select_list,
        const std::string& into,
        std::vector<Table> from,
        odb::sql::expression::SQLExpression *where,
        Expressions group_by,
        std::pair<Expressions,std::vector<bool> > order_by);

	SQLExpression* createColumn(
		const std::string& columnName,
		const std::string& bitfieldName,
		const SQLExpression* vectorIndex,
		const Table& table,
		const SQLExpression* pshift);

    eckit::DataHandle* implicitFromTableSource() { return implicitFromTableSource_; }
    void implicitFromTableSource(eckit::DataHandle* h) { implicitFromTableSource_ = h; }

    std::istream* implicitFromTableSourceStream() { return implicitFromTableSourceStream_; }
    void implicitFromTableSourceStream(std::istream* is) { implicitFromTableSourceStream_ = is; }

	SQLDatabase* database() { return database_; }
	void database(SQLDatabase* db) { database_ = db; }

	SQLOutputConfig config() { return config_; }
	void config(const SQLOutputConfig& cfg) { config_ = cfg; }

	std::string csvDelimiter() { return csvDelimiter_; }
	void csvDelimiter(const std::string& d) { csvDelimiter_ = d; }

    static odb::MetaData toODAColumns(odb::sql::SQLSession&, const odb::sql::TableDef&);

private:
    // No copy allowed
    SQLSelectFactory(const SQLSelectFactory&);
    SQLSelectFactory& operator=(const SQLSelectFactory&);

	std::string index(const std::string& columnName, const SQLExpression* index);

	void reshift(Expressions&);

	SQLExpression* reshift(SQLExpression*);

    //void resolveImplicitFrom(SQLSession&, std::vector<SQLTable*>& from);
    std::vector<SQLTable*> resolveImplicitFrom(SQLSession&, std::vector<Table>& from);

    eckit::DataHandle* implicitFromTableSource_;

    std::istream* implicitFromTableSourceStream_;

    //SchemaAnalyzer& analyzer();
    MetaData columns(const std::string& tableName);
    SQLOutput* createOutput(SQLSession&, const std::string& into, size_t orderBySize );

    SQLDatabase* database_;
    SQLOutputConfig config_;
    int maxColumnShift_;
    int minColumnShift_;
    std::string csvDelimiter_;

    //friend class eckit::NewAlloc0<SQLSelectFactory>;
};

} // namespace sql
} // namespace eckit

#endif
