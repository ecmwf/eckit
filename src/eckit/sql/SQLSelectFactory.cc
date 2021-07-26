/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/types/Types.h"
#include "eckit/utils/Translator.h"

#include "eckit/sql/SQLDistinctOutput.h"
#include "eckit/sql/SQLOrderOutput.h"
#include "eckit/sql/SQLOutputConfig.h"
#include "eckit/sql/SQLSelect.h"
#include "eckit/sql/SQLSelectFactory.h"
#include "eckit/sql/SQLSession.h"
#include "eckit/sql/expression/BitColumnExpression.h"
#include "eckit/sql/expression/SQLExpressions.h"
#include "eckit/sql/expression/ShiftedColumnExpression.h"
#include "eckit/sql/expression/function/FunctionExpression.h"


namespace eckit {
namespace sql {

//----------------------------------------------------------------------------------------------------------------------

SQLSelectFactory::SQLSelectFactory(SQLSession& session) :
    session_(session), database_(session.currentDatabase()), maxColumnShift_(0), minColumnShift_(0) {}

/*void SQLSelectFactory::reset()
{
    // TODO> we may need to delete things here...
    config_ = SQLOutputConfig::defaultConfig();
    maxColumnShift_ = 0;
    minColumnShift_ = 0;
}
*/

std::string SQLSelectFactory::index(const std::string& columnName, const SQLExpression* index) {
    if (index == 0)
        return columnName;

    bool missing    = false;
    std::string idx = Translator<int, std::string>()(int(index->eval(missing)));
    ASSERT(!missing);
    return columnName + "_" + idx;
}

std::shared_ptr<SQLExpression> SQLSelectFactory::createColumn(const std::string& columnName,
                                                              const std::string& bitfieldName,
                                                              std::shared_ptr<SQLExpression>& vectorIndex,
                                                              const std::string& tableReference,
                                                              std::shared_ptr<SQLExpression>& pshift) {
    if (!pshift->isConstant())
        throw eckit::UserError("Value of shift operator must be constant");
    bool missing = false;

    // Internally shift is an index in the cyclic buffer of old values, so the shift value is negative.
    int shift = -pshift->eval(missing);

    if (shift > maxColumnShift_)
        maxColumnShift_ = shift;
    if (shift < minColumnShift_)
        minColumnShift_ = shift;

    std::string expandedColumnName(index(columnName, vectorIndex.get()));
    // TODO: handle .<database>
    return bitfieldName.size()
               ? (shift == 0 ? std::make_shared<BitColumnExpression>(expandedColumnName, bitfieldName, tableReference)
                             : std::make_shared<ShiftedColumnExpression<BitColumnExpression>>(
                                 expandedColumnName, bitfieldName, tableReference, shift, -shift))
               : (shift == 0 ? std::make_shared<ColumnExpression>(expandedColumnName + tableReference, tableReference)
                             : std::make_shared<ShiftedColumnExpression<ColumnExpression>>(
                                 expandedColumnName + tableReference, tableReference, shift, -shift));
}

/*
SchemaAnalyzer& SQLSelectFactory::analyzer()
{ return SQLSession::current().currentDatabase().schemaAnalyzer(); }

MetaData SQLSelectFactory::columns(const std::string& tableName)
{
    const TableDef& tabledef ( enalyzer().findTable(tableName) );
    const ColumnDefs& columnDefs ( tabledef.columns() );

    //TODO: Convert ColumnDefs (from tabledef) into MetaData and push it into the SQLODAOutput
    ASSERT( false ); /// @todo this code must be fixed and return
}
*/


SQLSelect* SQLSelectFactory::create(bool distinct, const Expressions& select_list, const std::string& into,
                                    const std::vector<std::reference_wrapper<SQLTable>>& from,
                                    std::shared_ptr<SQLExpression> where, const Expressions& group_by,
                                    std::pair<Expressions, std::vector<bool>> order_by) {
    std::ostream& L(Log::debug());

    if (where)
        L << "SQLSelectFactory::create: where = " << *where << std::endl;

    SQLSelect* r(0);

    // Which tables are we selecting from?

    std::vector<std::reference_wrapper<const SQLTable>> fromTables;

    if (!from.size())
        L << "No from clause in SQL statement. Using implicit tables" << std::endl;
    for (const SQLTable& tbl : from.size() ? from : database_.implicitTables()) {
        fromTables.push_back(tbl);
    }

    // n.b. it is acceptable for fromTables.size() == 0, if the expressions
    //      are all constant. So we defer this check to later.

    // Expand any wildcards in the select statement

    Expressions select;
    for (Expressions::size_type i(0); i < select_list.size(); ++i) {
        L << "expandStars: " << *select_list[i] << std::endl;
        select_list[i]->expandStars(fromTables, select);
    }

    ASSERT(maxColumnShift_ >= 0);
    ASSERT(minColumnShift_ <= 0);
    if (minColumnShift_ < 0) {

        L << std::endl
          << "SELECT_LIST before reshifting:" << select << std::endl;
        select = select.reshift_expressions(minColumnShift_);
        L << std::endl
          << "SELECT_LIST after reshifting:" << select << std::endl;

        if (where) {
            L << std::endl
              << "WHERE before reshifting:" << *where << std::endl;
            where = where->reshift(minColumnShift_);
            L << std::endl
              << "WHERE after reshifting:" << *where << std::endl;
        }

        if (!order_by.first.empty()) {
            order_by.first = order_by.first.reshift_expressions(minColumnShift_);
        }
    }

    maxColumnShift_ = 0;
    minColumnShift_ = 0;

    if (group_by.size())
        Log::info() << "GROUP BY clause seen and ignored. Non aggregated values on select list will be used instead."
                    << std::endl;

    SQLOutput* outputEndpoint = 0;
    std::vector<std::unique_ptr<SQLOutput>> newOutputs;

    if (!into.empty()) {
        newOutputs.emplace_back(session_.newFileOutput(into));
        outputEndpoint = newOutputs.back().get();
    }
    else {
        outputEndpoint = &session_.output();
    }

    if (order_by.first.size()) {
        newOutputs.emplace_back(new SQLOrderOutput(*outputEndpoint, order_by));
        outputEndpoint = newOutputs.back().get();
    }
    if (distinct) {
        newOutputs.emplace_back(new SQLDistinctOutput(*outputEndpoint));
        outputEndpoint = newOutputs.back().get();
    }

    r = new SQLSelect(select, fromTables, where, *outputEndpoint, std::move(newOutputs));

    maxColumnShift_ = 0;
    return r;
}

// MetaData SQLSelectFactory::toODAColumns(SQLSession& session, const TableDef& tableDef)
//{
//    std::ostream& L(eckit::Log::debug());
//
//    ColumnDefs columnDefs (tableDef.columns());
//    MetaData md(0);
//    for (size_t i(0); i < columnDefs.size(); ++i)
//    {
//        ColumnDef& c (columnDefs[i]);
//        L << "   " << c.name() << ":" << c.type() << std::endl;
//
//        SchemaAnalyzer& a (session.currentDatabase().schemaAnalyzer());
//        if (a.isBitfield(c.name())) {
//            const BitfieldDef& bf ( a.getBitfieldTypeDefinition(c.name()) );
//            md.addBitfield(c.name(), bf );
//        }
//        else {
//            ColumnType type (Column::type(c.type()));
//            if (type == BITFIELD)
//                md.addBitfield(c.name(), c.bitfieldDef());
//            else
//                md.addColumn(c.name(), c.type());
//        }
//
//        ASSERT( &md[i]->coder() );
//    }
//    return md;
//}

// SQLOutput& SQLSelectFactory::createOutput (const std::string& into, size_t orderBySize)
//{
//// TODO: FIXME
////size_t maxOpenFiles ( ! context ? 100 : atoi(context->environment().lookup("maxOpenFiles", "100",
///*context).c_str()));
// size_t maxOpenFiles ( 100);
////TODO: pass parameter into to defaultFormat
// SQLOutput *r (NULL);
////if (config_.outputFormat() == "callback") return session.defaultOutput();

// std::string outputFile ((config_.outputFormat() == "odb") ? config_.outputFile() : into);
// Log::debug() << "SQLSelectFactory::createOutput: outputFile: '" << outputFile << "'" << std::endl;
// if (! outputFile.size())


//    TemplateParameters templateParameters;
//    TemplateParameters::parse(outputFile, templateParameters);
//    if (templateParameters.size())
//    {
//        NOTIMP;
////        r = new SQLODAOutput<DispatchingWriter>(new DispatchingWriter(outputFile, orderBySize ? 1 : maxOpenFiles));
//    }
//    else
//    {
//        SchemaAnalyzer& a (session.currentDatabase().schemaAnalyzer());
//        if (! a.tableKnown(outputFile)) {
//            NOTIMP;
////            r = new SQLODAOutput<Writer<> >(new Writer<>(outputFile));
//        } else
//        {
//            NOTIMP;
//            Log::info() << "Table in the INTO clause known (" << outputFile << ")" << std::endl;
////            const TableDef* tableDef (&a.findTable(outputFile));
////            r = new SQLODAOutput<Writer<> >(new Writer<>(outputFile), toODAColumns(session, *tableDef));
//        }
//    }
//    return r;
//}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace sql
}  // namespace eckit
