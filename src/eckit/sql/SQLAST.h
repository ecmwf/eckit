/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file SQLAST.h
/// @author Piotr Kuchta, ECMWF April 2009

#ifndef odb_api_SQLAST_H
#define odb_api_SQLAST_H

#include "eckit/eckit.h"
#include "eckit/sql/SQLTypedefs.h"
#include "odb_api/Expressions.h"

namespace eckit {
namespace sql {

typedef std::pair<long,long> Range;

class ColumnDef
{
public:
	ColumnDef();

	ColumnDef(const std::string& name, const std::string& type, const Range& range, const std::string& defaultValue);

    const std::string& name() const { return name_; }
    void name(const std::string& name) { name_ = name; }
    const std::string& type() const { return type_; }
    const Range& range() const { return range_; }
    const std::string& defaultValue() const { return defaultValue_; }
    bool hasDefaultValue() const { return hasDefaultValue_; }
    const BitfieldDef& bitfieldDef() const { return bitfieldDef_; }
    void bitfieldDef(const BitfieldDef& b) { bitfieldDef_ = b; }

private:
    std::string name_;
    std::string type_;
    Range range_;
    std::string defaultValue_;
    bool hasDefaultValue_;
    BitfieldDef bitfieldDef_;
};

typedef std::vector<ColumnDef> ColumnDefs;

class ConstraintDef
{
    enum Type { NONE, PRIMARY_KEY, FOREIGN_KEY };
public:
	ConstraintDef();

    ConstraintDef(const std::string& name, const std::vector<std::string>& primaryKey); 

    ConstraintDef(const std::string& name,
        const std::vector<std::string>& foreignKey,
        const std::string& relatedTable,
        const std::vector<std::string>& relatedColumn); 

    bool isPrimaryKey() const { return type_ == PRIMARY_KEY; }
    bool isForeignKey() const { return type_ == FOREIGN_KEY; }

    const std::string& name() const { return name_; }
    const std::vector<std::string>& columns() const { return columns_; }
    const std::string& relatedTable() const { return relatedTable_; }
    const std::vector<std::string>& relatedColumns() const { return relatedColumns_; }
private:
    Type type_;
    std::string name_;
    std::vector<std::string> columns_;
    std::string relatedTable_;
    std::vector<std::string> relatedColumns_;
};

typedef std::vector<ConstraintDef> ConstraintDefs;

class TableDef
{
public:
    TableDef();
    TableDef(const std::string& name, 
             const ColumnDefs& columns,
             const ConstraintDefs& constraints, 
             const std::vector<std::string>& parents,
             const std::string& location);

    const std::string& name() const { return name_; }
    void name(const std::string& name) { name_ = name; }
    ColumnDefs& columns() { return columns_; }
	const ColumnDefs& columns() const { return columns_; }
    const ConstraintDefs& constraints() const { return constraints_; }
    const std::vector<std::string>& parents() const { return parents_; }
    const std::string location() const { return location_; }
private:
    std::string name_;
    ColumnDefs columns_;
    ConstraintDefs constraints_;
	std::vector<std::string> parents_;
    std::string location_;
};

typedef std::map<std::string, TableDef> TableDefs;

class SchemaDef
{
public:
	SchemaDef();
	SchemaDef(const TableDefs& tables);
	TableDefs& tables() { return tables_; }
	const TableDefs& tables() const { return tables_; }
private:
	TableDefs tables_;
};

typedef std::map<std::string, SchemaDef> SchemaDefs;

class Definitions
{
public:
    Definitions();
    Definitions(const SchemaDefs& schemas, const TableDefs& tables);
    const SchemaDefs& schemas() const { return schemas_; }
    const TableDefs& tables() const { return tables_; }
private:
    SchemaDefs schemas_;
    TableDefs tables_;
};

struct Table {
    Table() : name(), database(), embeddedCode(), dataDescriptor() {}

    Table(const std::string& n, const std::string& db, const bool c)
    : name(n), database(db), embeddedCode(c), dataDescriptor() {}

    Table(const std::string& n, const std::string& db, const bool c, const bool d)
    : name(n), database(db), embeddedCode(c), dataDescriptor(d) {}

    Table(const Table& o)
    : name(o.name), 
      database(o.database), 
      embeddedCode(o.embeddedCode),
      dataDescriptor(o.dataDescriptor)
    {}

    friend std::ostream& operator<< (std::ostream& s, const Table& t)
    {
        return s << "[table,name=" << t.name 
                 << ",database=" << t.database 
                 << ",embeddedCode=" << t.embeddedCode 
                 << ",dataDescriptor=" << t.dataDescriptor 
                 << "]"
                 //<< std::endl
                 ;
    }

    std::string name;
    std::string database;
    bool embeddedCode;   // data to be computed
    bool dataDescriptor; // we got something in quotes, e.g. file name, instead of identifier (table name)
};

struct SelectAST {
    SelectAST(
        bool                                             distinct,
        bool                                             all,
        const Expressions&                               selectList,
        const std::string&                               into,
        const std::vector<Table>&                        from,
        SQLExpression*                                   where,
        const Expressions&                               groupBy,
        const std::pair<Expressions,std::vector<bool> >& orderBy )

    : distinct(distinct),
      all(all),
      selectList(selectList),
      into(into),
      from(from),
      where(where),
      groupBy(groupBy),
      orderBy(orderBy)
    {}

    SelectAST(const SelectAST& o)
    : distinct(o.distinct),
      all(o.all),
      selectList(o.selectList),
      into(o.into),
      from(o.from),
      where(o.where),
      groupBy(o.groupBy),
      orderBy(o.orderBy)
    {}

    SelectAST() {}

    bool                                      distinct;
    bool                                      all;
    Expressions                               selectList;
    std::string                               into;
    std::vector<Table>                        from;
    SQLExpression*                            where;
    Expressions                               groupBy;
    std::pair<Expressions,std::vector<bool> > orderBy;
};

struct InsertAST {
    InsertAST () {}

    InsertAST (const Table& table, const std::vector<std::string>& columns, const std::vector<std::string>& values)
    : table_(table),
      columns_(columns),
      values_(values)
    {}

    InsertAST (const InsertAST& other) 
    : table_(other.table_), 
      columns_ (other.columns_), 
      values_(other.values_) 
    {}

    Table table_;
    std::vector<std::string> columns_;
    std::vector<std::string> values_;
};

struct EmbeddedAST {
    EmbeddedAST() {}

    EmbeddedAST (const std::string& s) : code_ (s) {}
    EmbeddedAST (const EmbeddedAST& ast) : code_ (ast.code_) {}

    std::string code_;
};

} // namespace sql
} // namespace eckit

#endif
