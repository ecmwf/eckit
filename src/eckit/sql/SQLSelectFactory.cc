/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/utils/Translator.h"
#include "eckit/types/Types.h"

#include "eckit/sql/SQLSelectFactory.h"
#include "odb_api/DispatchingWriter.h"
#include "odb_api/FunctionExpression.h"
#include "eckit/sql/SQLDistinctOutput.h"
#include "odb_api/SQLODAOutput.h"
#include "eckit/sql/SQLOrderOutput.h"
#include "eckit/sql/SQLSelect.h"
#include "eckit/sql/SQLSession.h"
#include "odb_api/TemplateParameters.h"
#include "odb_api/Writer.h"
#include "eckit/sql/SQLAST.h"
#include "eckit/sql/SQLOutputConfig.h"
#include "odb_api/BitColumnExpression.h"
#include "odb_api/ShiftedColumnExpression.h"

using namespace eckit;
using namespace std;

namespace eckit {
namespace sql {

SQLSelectFactory::SQLSelectFactory(const odb::sql::SQLOutputConfig& config, const std::string& csvDelimiter)
: implicitFromTableSource_(0),
  implicitFromTableSourceStream_(0),
  database_(0),
  config_(config), //SQLOutputConfig::defaultConfig()),
  maxColumnShift_(0),
  minColumnShift_(0),
  csvDelimiter_(csvDelimiter) //",")
{}

/*void SQLSelectFactory::reset()
{
    // TODO> we may need to delete things here...
    implicitFromTableSource_ = 0;
    implicitFromTableSourceStream_ = 0;
    database_ = 0;
    config_ = SQLOutputConfig::defaultConfig();
    maxColumnShift_ = 0;
    minColumnShift_ = 0;
    //csvDelimiter_ = ",";
}
*/

string SQLSelectFactory::index(const string& columnName, const SQLExpression* index)
{
	if (index == 0)
		return columnName;

	bool missing = false;
	string idx = Translator<int,string>()(int(index->eval(missing)));
	ASSERT(! missing);
	return columnName + "_" + idx;
}

SQLExpression* SQLSelectFactory::createColumn(
	const string& columnName,
	const string& bitfieldName,
	const SQLExpression* vectorIndex,
	const Table& table,
	const SQLExpression* pshift)
{
	if (! pshift->isConstant()) throw eckit::UserError("Value of shift operator must be constant");
	bool missing = false;
	
	// Internally shift is an index in the cyclic buffer of old values, so the shift value is negative.
	int shift = - pshift->eval(missing);

	if (shift > maxColumnShift_) maxColumnShift_ = shift;
	if (shift < minColumnShift_) minColumnShift_ = shift;

	string expandedColumnName( index(columnName, vectorIndex) );
    // TODO: handle .<database>
	return bitfieldName.size()
		? (shift == 0 ? new BitColumnExpression(expandedColumnName, bitfieldName, table.name)
					  : new ShiftedColumnExpression<BitColumnExpression>(expandedColumnName, bitfieldName, table.name, shift, -shift))
		: (shift == 0 ? new ColumnExpression(expandedColumnName + table.name, table.name)
					  : new ShiftedColumnExpression<ColumnExpression>(expandedColumnName + table.name, table.name, shift, -shift));
}

SQLExpression* SQLSelectFactory::reshift(SQLExpression* e)
{
    if (e == 0) return 0;
    SQLExpression* r = e;
    ShiftedColumnExpression<BitColumnExpression>* c1 = dynamic_cast<ShiftedColumnExpression<BitColumnExpression>*>(e);
    if (c1) {
        int newShift = c1->shift() - minColumnShift_;
        ASSERT(newShift >= 0);
        r = newShift > 0
            ? new ShiftedColumnExpression<BitColumnExpression>(*c1, newShift, c1->nominalShift())
            : (new BitColumnExpression(*c1))->nominalShift(c1->nominalShift());
        delete c1;
        return r;
    } 

    ShiftedColumnExpression<ColumnExpression>* c2 = dynamic_cast<ShiftedColumnExpression<ColumnExpression>*>(e);
    if (c2) {
        int newShift = c2->shift() - minColumnShift_ ;
        ASSERT(newShift >= 0);
        r = newShift > 0
            ? new ShiftedColumnExpression<ColumnExpression>(*c2, newShift, c2->nominalShift())
            : (new ColumnExpression(*c2))->nominalShift(c2->nominalShift());
        delete c2;
        return r;
    } 

    BitColumnExpression* c3 = dynamic_cast<BitColumnExpression*>(e);
    if(c3) {
        r = new ShiftedColumnExpression<BitColumnExpression>(*c3, -minColumnShift_, 0);
        delete c3;
        return r;
    }

    ColumnExpression* c4 = dynamic_cast<ColumnExpression*>(e);
    if(c4) {
        r = new ShiftedColumnExpression<ColumnExpression>(*c4, -minColumnShift_, 0);
        delete c4;
        return r;
    }
    
    odb::sql::expression::function::FunctionExpression* f = dynamic_cast<odb::sql::expression::function::FunctionExpression*>(e);
    if (f) {
        reshift(f->args());
        return r;
    }

    Log::info() << "SQLSelectFactory::reshift: SKIP " << *e << endl;
    return r;
}

void SQLSelectFactory::reshift(Expressions& select)
{
    ostream& L(Log::debug());
	L << "reshift: maxColumnShift_ = " << maxColumnShift_ << endl;
	L << "reshift: minColumnShift_ = " << minColumnShift_ << endl;
	for (size_t i = 0; i < select.size(); ++i)
		L << "reshift: <- select[" << i << "]=" << *select[i] << endl;

	for (size_t i = 0; i < select.size(); ++i)
        select[i] = reshift(select[i]);

	L << endl;
	for (size_t i = 0; i < select.size(); ++i)
		L << "reshift: -> select[" << i << "]=" << *select[i] << endl;
}

//void SQLSelectFactory::resolveImplicitFrom(SQLSession& session, vector<SQLTable*>& from)
vector<SQLTable*> SQLSelectFactory::resolveImplicitFrom(SQLSession& session, vector<Table>& from)
{
    ostream& L (Log::debug());
    
    L << "No <from> clause" << endl;

    // TODO: SQLTable => string. 

    SQLTable* table = implicitFromTableSource_ ? session.openDataHandle(*implicitFromTableSource_)
        : implicitFromTableSourceStream_ ? session.openDataStream(*implicitFromTableSourceStream_, csvDelimiter_) 
        //: database_ ? database_->table("defaultTable")
        : database_ ? database_->defaultTable()
        : session.currentDatabase().dualTable();

    L << "Implicit FROM: " << *table << endl;

    vector<SQLTable*> fromTables;
    fromTables.push_back(table);
    return fromTables;
}

/*
SchemaAnalyzer& SQLSelectFactory::analyzer()
{ return SQLSession::current().currentDatabase().schemaAnalyzer(); }

MetaData SQLSelectFactory::columns(const string& tableName)
{
    const TableDef& tabledef ( enalyzer().findTable(tableName) );
    const ColumnDefs& columnDefs ( tabledef.columns() );

    //TODO: Convert ColumnDefs (from tabledef) into MetaData and push it into the SQLODAOutput
    ASSERT( false ); /// @todo this code must be fixed and return
}
*/

SQLSelect* SQLSelectFactory::create (SQLSession& session, const SelectAST& a)
{
    return session.selectFactory().create(session, a.distinct, a.all, a.selectList, a.into, a.from, a.where, a.groupBy, a.orderBy);
}

SQLSelect* SQLSelectFactory::create (
    odb::sql::SQLSession& session,
    bool distinct,
    bool all,
    Expressions select_list,
    const string& into,
    //vector<SQLTable*> from,
    vector<Table> from,
    SQLExpression *where,
    Expressions group_by,
    pair<Expressions,vector<bool> > order_by)
{
    ostream& L(Log::debug());

	if (where) L << "SQLSelectFactory::create: where = " << *where << endl;

	SQLSelect* r (0);
	//SQLSession& session (SQLSession::current());

    std::vector<SQLTable*> fromTables;

	if (! from.size()) 
    {
        std::vector<SQLTable*> implicitTables (resolveImplicitFrom(session, from));
        fromTables.insert( fromTables.begin(), implicitTables.begin(), implicitTables.end() );
    }

    //table : IDENT '.' IDENT { SQLSession& s  = SQLSession::current(); $$ = s.findTable($1,$3); }
    for (size_t i(0); i < from.size(); ++i)
    {
        Table& t (from[i]);

        ASSERT(! t.embeddedCode);
        fromTables.push_back(session.findTable(t));
    }

	Expressions select;
	for (ColumnDefs::size_type i (0); i < select_list.size(); ++i)
	{
		L << "expandStars: " << *select_list[i] << endl;
		select_list[i]->expandStars(fromTables, select);
	}

	ASSERT(maxColumnShift_ >= 0);
	ASSERT(minColumnShift_ <= 0);
	if (minColumnShift_ < 0) 
    {
        L << endl << "SELECT_LIST before reshifting:" << select << endl;
		reshift(select);
        L << "SELECT_LIST after reshifting:" << select << endl << endl;

        if (where)
        {
            L << endl << "WHERE before reshifting:" << *where << endl;
            where = reshift(where);
            L << "WHERE after reshifting:" << *where << endl << endl;
        }

        reshift(order_by.first);
    }

	maxColumnShift_ = 0;
	minColumnShift_ = 0;

	if (group_by.size())
		Log::info() << "GROUP BY clause seen and ignored. Non aggregated values on select list will be used instead." << endl;

    SQLOutput *out (createOutput(session, into, order_by.first.size()));

    if(distinct)              { out = new SQLDistinctOutput(out); }
    if(order_by.first.size()) { out = new SQLOrderOutput(out, order_by); }
    r = new SQLSelect(select, fromTables, where, out, config_, all);

	maxColumnShift_ = 0;
	return r;
}

MetaData SQLSelectFactory::toODAColumns(odb::sql::SQLSession& session, const odb::sql::TableDef& tableDef)
{
    ostream& L(eckit::Log::debug());

    odb::sql::ColumnDefs columnDefs (tableDef.columns());
    MetaData md(0); 
    for (size_t i(0); i < columnDefs.size(); ++i)
    {
        odb::sql::ColumnDef& c (columnDefs[i]);
        L << "   " << c.name() << ":" << c.type() << endl; 

        SchemaAnalyzer& a (session.currentDatabase().schemaAnalyzer());
        if (a.isBitfield(c.name())) {
            const BitfieldDef& bf ( a.getBitfieldTypeDefinition(c.name()) );
            md.addBitfield(c.name(), bf ); 
        }
        else {
            ColumnType type (Column::type(c.type()));
            if (type == BITFIELD)
                md.addBitfield(c.name(), c.bitfieldDef());
            else
                md.addColumn(c.name(), c.type());
        }

        ASSERT( &md[i]->coder() );
    }
    return md;
}

SQLOutput* SQLSelectFactory::createOutput (SQLSession& session, const string& into, size_t orderBySize)
{
    // TODO: FIXME
    //size_t maxOpenFiles ( ! context ? 100 : atoi(context->environment().lookup("maxOpenFiles", "100", *context).c_str()));
    size_t maxOpenFiles ( 100);
    //TODO: pass parameter into to defaultFormat
    SQLOutput *r (NULL);
    //if (config_.outputFormat() == "callback") return session.defaultOutput();

    string outputFile ((config_.outputFormat() == "odb") ? config_.outputFile() : into);
    Log::debug() << "SQLSelectFactory::createOutput: outputFile: '" << outputFile << "'" << endl;
    if (! outputFile.size())
        return r = session.defaultOutput();

    TemplateParameters templateParameters;
    TemplateParameters::parse(outputFile, templateParameters);
    if (templateParameters.size())
    {
        r = new SQLODAOutput<DispatchingWriter>(new DispatchingWriter(outputFile, orderBySize ? 1 : maxOpenFiles));
    } 
    else 
    {
        SchemaAnalyzer& a (session.currentDatabase().schemaAnalyzer());
        if (! a.tableKnown(outputFile)) 
            r = new SQLODAOutput<Writer<> >(new Writer<>(outputFile));
        else
        {
            Log::info() << "Table in the INTO clause known (" << outputFile << ")" << endl;
            const odb::sql::TableDef* tableDef (&a.findTable(outputFile));
            r = new SQLODAOutput<Writer<> >(new Writer<>(outputFile), toODAColumns(session, *tableDef));
        } 
    }
    return r;
}

} // namespace sql
} // namespace eckit
